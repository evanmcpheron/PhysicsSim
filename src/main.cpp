#include <iostream>
#include <fstream>
#include <iomanip>
#include "OrbitalBody/OrbitalBody.h"
#include "ThrustModel/ThrustModel.h"
#include "Vessel/Vessel.h"

int main()
{
    // === Setup Orbital Bodies ===
    OrbitalBody earth(5.972e24, 6.371e6); // kg, meters
    OrbitalBody moon(7.342e22, 1.7371e6); // kg, meters

    // Switch between Earth or Moon by changing this line:
    OrbitalBody *currentBody = &earth;
    std::string bodyName = "Earth";

    // === Setup Engine ===
    ThrustModel engine(
        1.5e6, // Max thrust in Newtons (1.5 MN)
        350.0, // ISP vacuum
        280.0  // ISP sea level
    );

    // === Setup Vessel ===
    Vessel rocket(
        0.0,     // altitude
        0.0,     // velocity
        10000.0, // dry mass (kg)
        20000.0, // fuel mass (kg)
        currentBody,
        engine);

    rocket.SetThrottle(0.1); // full throttle

    // === Simulation Settings ===
    const double deltaTime = 0.1;   // seconds per step
    const double totalTime = 120.0; // max simulation time
    double time = 0.0;

    // === Open log file ===
    std::ofstream logFile("flight_log.csv");
    logFile << "Time (s),Altitude (m),Velocity (m/s),Mass (kg),Fuel (%)\n";

    std::cout << "🚀 Launching from " << bodyName << "... Logging to flight_log.csv\n";

    while (time <= totalTime)
    {
        rocket.Update(deltaTime);

        logFile << std::fixed << std::setprecision(2)
                << time << ","
                << rocket.GetAltitude() << ","
                << rocket.GetVelocity() << ","
                << rocket.GetMass() << ","
                << rocket.GetFuelPercent() << "\n";

        time += deltaTime;

        if (rocket.GetFuelMass() <= 0.0)
        {
            std::cout << "🛑 Fuel exhausted at t = " << time << " seconds\n";
            break;
        }
    }

    logFile.close();
    std::cout << "✅ Simulation complete.\n";

    return 0;
}
