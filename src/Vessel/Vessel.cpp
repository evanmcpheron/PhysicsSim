#include "Vessel.h"
#include "../OrbitalBody/OrbitalBody.h" // include full type now

Vessel::Vessel(double startingAltitude,
               double startingVelocity,
               double dryMassKg,
               double fuelMassKg,
               double dragCoefficient,
               double crossSectionArea,
               OrbitalBody *parentBody,
               const ThrustModel &engineModel)
    : altitudeMeters(startingAltitude),
      velocityMetersPerSecond(startingVelocity),
      dryMassKg(dryMassKg),
      fuelMassKg(fuelMassKg),
      initialFuelMassKg(fuelMassKg),
      dragCoefficient(dragCoefficient),
      crossSectionArea(crossSectionArea),
      parentBody(parentBody),
      engine(engineModel),
      totalHeatLoad(0.0),
      currentHeatRate(0.0),
      lastAirDensity(0.0),
      lastDragForce(0.0),
      lastDragAcceleration(0.0) {}

// ==============================
// Update per time step
// ==============================
void Vessel::Update(double deltaTime)
{
    double altitude = altitudeMeters;
    double gravity = parentBody->ComputeGravitationalAcceleration(altitude);
    double pressure = parentBody->ComputeAtmosphericPressure(altitude);

    // === Thrust ===
    if (fuelMassKg > 0.0)
    {
        double thrust = engine.ComputeThrust(pressure);
        double acceleration = thrust / GetMass();
        velocityMetersPerSecond += acceleration * deltaTime;

        double massFlowRate = engine.ComputeMassFlowRate(pressure);
        double fuelUsed = massFlowRate * deltaTime;
        fuelMassKg -= std::min(fuelUsed, fuelMassKg);
    }

    // === Atmospheric Drag ===
    lastAirDensity = 0.0;
    lastDragForce = 0.0;
    lastDragAcceleration = 0.0;

    if (Atmosphere *atm = parentBody->GetAtmosphere())
    {
        lastAirDensity = atm->GetDensity(altitudeMeters);
        lastDragForce = atm->ComputeDragForce(
            altitudeMeters,
            velocityMetersPerSecond,
            dragCoefficient,
            crossSectionArea);

        // Drag always opposes velocity
        lastDragAcceleration = lastDragForce / GetMass();
        double dragDirection = (velocityMetersPerSecond > 0.0) ? -1.0 : 1.0;
        velocityMetersPerSecond += dragDirection * lastDragAcceleration * deltaTime;
    }

    // === Reentry Heating ===
    if (parentBody->GetAtmosphere())
    {
        double rho = lastAirDensity;
        double velocity = std::abs(velocityMetersPerSecond);

        constexpr double heatTransferCoefficient = 1.83e-4;

        currentHeatRate = heatTransferCoefficient * rho * velocity * velocity * velocity;
        totalHeatLoad += currentHeatRate * deltaTime;
    }
    else
    {
        currentHeatRate = 0.0;
    }

    // === Radiative Cooling ===
    constexpr double emissivity = 0.85;                // typical surface (0.0–1.0)
    constexpr double stefanBoltzmann = 5.670374419e-8; // W/(m²·K⁴)
    constexpr double heatCapacityPerArea = 2000.0;     // J/(m²·K), assume bulk property

    // Estimate surface temperature from heat load (optional, simplified)
    surfaceTemperature = std::pow(
        std::max(0.0, totalHeatLoad) / heatCapacityPerArea,
        1.0 / 1.0 // exponent 1 for now; will refine next step
    );

    // Radiated power per m²
    double radiatedPower = emissivity * stefanBoltzmann * std::pow(surfaceTemperature, 4.0);

    // Subtract radiated energy
    totalHeatLoad = std::max(0.0, totalHeatLoad - radiatedPower * deltaTime);

    // === Gravity ===
    velocityMetersPerSecond -= gravity * deltaTime;

    // === Update Altitude ===
    altitudeMeters += velocityMetersPerSecond * deltaTime;
}

// ==============================
// Set throttle
// ==============================
void Vessel::SetThrottle(double throttle)
{
    engine.SetThrottle(throttle);
}

// ==============================
// Getters
// ==============================
double Vessel::GetAltitude() const
{
    return altitudeMeters;
}

double Vessel::GetVelocity() const
{
    return velocityMetersPerSecond;
}

double Vessel::GetMass() const
{
    return dryMassKg + fuelMassKg;
}

double Vessel::GetFuelMass() const
{
    return fuelMassKg;
}

double Vessel::GetFuelPercent() const
{
    if (initialFuelMassKg <= 0.0)
        return 0.0;
    return (fuelMassKg / initialFuelMassKg) * 100.0;
}

double Vessel::GetHeatRate() const
{
    return currentHeatRate;
}

double Vessel::GetTotalHeatLoad() const
{
    return totalHeatLoad;
}