#include <iostream>
#include <fstream>
#include <iomanip>
#include "OrbitalBody/OrbitalBody.h"
#include "Atmosphere/Atmosphere.h"
#include "ThrustModel/ThrustModel.h"
#include "Vessel/Vessel.h"

void SimulateLaunch(const std::string &bodyName, OrbitalBody *body)
{
    // === Setup Engine ===
    ThrustModel engine(1.5e6, 350.0, 280.0); // N, ISP (vac, sea level)

    // Aerodynamic properties for a slender rocket shape
    double dragCoefficient = 2.0;  // dimensionless (lower = more streamlined)
    double referenceArea_m2 = 1.2; // cross-sectional area in square meters

    // === Setup Vessel ===
    Vessel rocket(
        0.0,     // altitude (m)
        0.0,     // velocity (m/s)
        10000.0, // dry mass (kg)
        20000.0, // fuel mass (kg)
        dragCoefficient,
        referenceArea_m2,
        body,
        engine);
    rocket.SetThrottle(1.0);

    // === Logging Setup ===
    std::string logFileName = "flight_log_" + bodyName + ".csv";
    std::ofstream logFile(logFileName);
    logFile << "Time (s),Altitude (m),Velocity (m/s),Mass (kg),Fuel (%),Air Density (kg/m^3),Drag Force (N),Drag Accel (m/s^2)\n";

    // === Simulation Parameters ===
    const double deltaTime = 0.1;
    const double totalTime = 600.0;
    double time = 0.0;

    double maxAltitude = 0.0;
    bool fuelBurnedOut = false;
    bool apexReached = false;

    std::cout << "🚀 Launching from " << bodyName << "... Logging to " << logFileName << "\n";

    while (time <= totalTime)
    {
        rocket.Update(deltaTime);

        double altitude = rocket.GetAltitude();
        double velocity = rocket.GetVelocity();

        // Track maximum altitude
        if (altitude > maxAltitude)
            maxAltitude = altitude;

        // Burnout detection
        if (rocket.GetFuelMass() <= 0.0 && !fuelBurnedOut)
        {
            fuelBurnedOut = true;
            std::cout << "🛑 Fuel exhausted at t = " << time << " seconds\n";
        }

        // Apex detection
        if (fuelBurnedOut && velocity <= 0.0 && !apexReached)
        {
            apexReached = true;
            std::cout << "📍 Apex reached at t = " << time << " seconds\n";
            std::cout << "🛰  Max Altitude: " << maxAltitude << " meters\n";
            break;
        }

        logFile << std::fixed << std::setprecision(2)
                << time << ","
                << rocket.GetAltitude() << ","
                << rocket.GetVelocity() << ","
                << rocket.GetMass() << ","
                << rocket.GetFuelPercent() << ","
                << rocket.GetLastAirDensity() << ","
                << rocket.GetLastDragForce() << ","
                << (rocket.GetLastDragForce() / rocket.GetMass()) << "\n";

        time += deltaTime;
    }

    logFile.close();
    std::cout << "✅ Simulation complete for " << bodyName << ".\n\n";
}

int main()
{
    // === Define Atmospheres ===
    Atmosphere earthAtmo(101325.0, 288.15, 0.0065, 0.0289644); // P0, T0, L, M
    Atmosphere marsAtmo(610.0, 210.0, 0.0045, 0.04401);        // Thin CO₂-rich

    // === Define Orbital Bodies ===
    OrbitalBody earth(5.972e24, 6.371e6, &earthAtmo);
    OrbitalBody mars(5.972e24, 6.371e6, &marsAtmo);

    // === Run simulations ===
    SimulateLaunch("Earth", &earth);
    SimulateLaunch("Mars", &mars);

    return 0;
}
