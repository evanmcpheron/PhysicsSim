#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
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
    logFile << "Time (s),Altitude (m),Velocity (m/s),Air Density (kg/m^3),"
            << "Drag Force (N),Drag Accel (m/s^2),Heat Rate (W/m^2),"
            << "Total Heat (J/m^2),Surface Temp (K)"
            << ",Heat Shield Temp (K),Heat Shield Mass (kg),Ablated Mass (kg),Shield Depleted"
            << ",Lift Force (N),Lift Vector Y" << "\n";

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
                << rocket.GetLastAirDensity() << ","
                << rocket.GetLastDragForce() << ","
                << rocket.GetLastDragAcceleration() << ","
                << rocket.GetHeatRate() << ","
                << rocket.GetTotalHeatLoad() << ","
                << rocket.GetSurfaceTemperature() << ","
                << rocket.GetHeatShieldSurfaceTemp() << ","
                << rocket.GetHeatShieldMass() << ","
                << rocket.GetAblatedMass() << ","
                << (rocket.IsHeatShieldDepleted() ? 1 : 0)
                << rocket.GetLiftForce() << "," << rocket.GetLiftVector().y << "\n";

        time += deltaTime;
    }

    logFile.close();
    std::cout << "✅ Simulation complete for " << bodyName << ".\n\n";
}

void SimulateReentry(const std::string &bodyName, OrbitalBody *body)
{
    // === No thrust model needed (no propulsion during reentry) ===
    ThrustModel dummyEngine(0.0, 0.0, 0.0);

    // === Capsule-like vessel for reentry ===
    double dragCoefficient = 1.25; // blunt body
    double crossSectionArea = 5.0; // m²

    Vessel capsule(
        100000.0, // Altitude: 100 km
        -7500.0,  // Velocity: 7.5 km/s downward
        5000.0,   // Dry mass
        0.0,      // No fuel
        dragCoefficient,
        crossSectionArea,
        body,
        dummyEngine);

    // === Add heat shield ===
    HeatShield shield(
        250.0, // Mass (kg)
        5.0,   // Area (m²)
        2e6    // J/kg
    );
    capsule.AttachHeatShield(&shield);

    // === Log Setup ===
    std::ofstream logFile("reentry_test_" + bodyName + ".csv");
    logFile << "Time (s),Altitude (m),Velocity (m/s),Air Density (kg/m^3),"
            << "Drag Force (N),Drag Accel (m/s^2),Heat Rate (W/m^2),"
            << "Total Heat (J/m^2),Surface Temp (K),"
            << "Heat Shield Temp (K),Heat Shield Mass (kg),Ablated Mass (kg),Shield Depleted,"
            << "Lift Force (N),Lift Vector Y\n";

    // === Sim Settings ===
    const double deltaTime = 0.1;
    const double maxTime = 600.0;
    double time = 0.0;

    std::cout << "🔥 Reentry simulation started...\n";

    while (time <= maxTime && capsule.GetAltitude() > 0.0)
    {
        capsule.Update(deltaTime);

        logFile << std::fixed << std::setprecision(2)
                << time << ","
                << capsule.GetAltitude() << ","
                << capsule.GetVelocity() << ","
                << capsule.GetLastAirDensity() << ","
                << capsule.GetLastDragForce() << ","
                << capsule.GetLastDragAcceleration() << ","
                << capsule.GetHeatRate() << ","
                << capsule.GetTotalHeatLoad() << ","
                << capsule.GetSurfaceTemperature() << ","
                << capsule.GetHeatShieldSurfaceTemp() << ","
                << capsule.GetHeatShieldMass() << ","
                << capsule.GetAblatedMass() << ","
                << (capsule.IsHeatShieldDepleted() ? 1 : 0) << ","
                << capsule.GetLiftForce() << "," << capsule.GetLiftVector().y << "\n";

        time += deltaTime;
    }

    logFile.close();
    std::cout << "✅ Reentry complete. Data saved to reentry_" + bodyName + "_test.csv\n";
}

void TestLiftForce(OrbitalBody *planet)
{
    std::cout << "🧪 Testing lift force at AoA and atmospheric conditions...\n";

    // Use dummy thrust model
    ThrustModel dummyEngine(0.0, 0.0, 0.0);

    // Setup a test vessel
    double altitude = 30000.0; // 30 km
    double velocity = -2500.0; // high reentry speed
    double dragCoefficient = 1.25;
    double crossSectionArea = 5.0;

    Vessel capsule(
        altitude,
        velocity,
        5000.0, // dry mass
        0.0,    // no fuel
        dragCoefficient,
        crossSectionArea,
        planet,
        dummyEngine);

    capsule.SetOrientationVector(Vector3(0.0, -1.0, 1.0).Normalized());

    capsule.SetThrottle(0.0); // ensure it's passive
    capsule.Update(0.1);      // simulate one step (can repeat if needed)

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "  Altitude: " << capsule.GetAltitude() << " m\n";
    std::cout << "  Velocity: " << capsule.GetVelocity() << " m/s\n";
    std::cout << "  AoA: " << capsule.GetAngleOfAttackDegrees() << "°\n";
    std::cout << "  Air Density: " << capsule.GetLastAirDensity() << " kg/m³\n";
    std::cout << "  Lift Force: " << capsule.GetLiftForce() << " N\n";
    std::cout << "  Lift Vector: (" << capsule.GetLiftVector().x << ", "
              << capsule.GetLiftVector().y << ", "
              << capsule.GetLiftVector().z << ")\n";
    std::cout << "✅ Lift test complete.\n";
}

struct TestCase
{
    std::string label;
    double velocity;
    double shieldMass;
    bool disableHeatShield;
};

void TestReentryOutcomes(OrbitalBody *planet)
{

    std::vector<TestCase> tests = {
        {"Safe Landing", -300.0, 250.0, false},
        {"Burnup", -7500.0, 0.0, true},
        {"Crash", -300.0, 250.0, false} // but impact too fast
    };

    for (const auto &test : tests)
    {
        std::cout << "\n🧪 Running: " << test.label << "\n";

        ThrustModel dummyEngine(0.0, 0.0, 0.0);

        Vessel capsule(
            100000.0,      // Altitude: 100 km
            test.velocity, // Entry speed
            5000.0,        // Dry mass
            0.0,           // No fuel
            1.25,          // Cd
            5.0,           // Area
            planet,
            dummyEngine);

        if (!test.disableHeatShield)
        {
            HeatShield shield(test.shieldMass, 5.0, 2e6);
            capsule.AttachHeatShield(&shield);
        }

        capsule.SetOrientationVector(Vector3(0.0, -1.0, 0.0)); // Straight down
        Parachute chute(
            500.0,  // Area in m²
            2.2,    // Cd — typical for round parachutes
            3000.0, // Deploy altitude
            8000.0  // Max supported mass
        );

        capsule.AttachParachute(&chute);
        const double deltaTime = 0.1;
        double time = 0.0;

        while (time <= 6000 && capsule.GetAltitude() > 0.0)
        {
            capsule.Update(deltaTime);
            time += deltaTime;
        }

        std::cout << "  Final altitude: " << capsule.GetAltitude() << " m\n";
        std::cout << "  Final velocity: " << capsule.GetVelocity() << " m/s\n";
        std::cout << "  Final surface temp: " << capsule.GetSurfaceTemperature() << " K\n";

        if (capsule.HasBurnedUp())
            std::cout << "  🔥 Burned up during reentry.\n";
        else if (capsule.HasCrashed())
            std::cout << "  💥 Crashed on impact.\n";
        else if (capsule.HasLandedSafely())
            std::cout << "  ✅ Landed safely.\n";
        else
            std::cout << "  ❓ Status unknown.\n";
    }
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

    SimulateReentry("Earth", &earth);
    SimulateReentry("Mars", &mars);
    std::cout << "🚀 All simulations completed.\n";

    TestLiftForce(&earth);
    TestLiftForce(&mars);

    TestReentryOutcomes(&earth);

    return 0;
}
