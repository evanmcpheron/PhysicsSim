#include "Vessel.h"
#include "../OrbitalBody/OrbitalBody.h" // include full type now

Vessel::Vessel(double startingAltitude,
               double startingVelocity,
               double dryMassKg,
               double fuelMassKg,
               OrbitalBody *parentBody,
               const ThrustModel &engineModel)
    : altitudeMeters(startingAltitude),
      velocityMetersPerSecond(startingVelocity),
      dryMassKg(dryMassKg),
      fuelMassKg(fuelMassKg),
      initialFuelMassKg(fuelMassKg),
      parentBody(parentBody),
      engine(engineModel) {}

// ==============================
// Update per time step
// ==============================
void Vessel::Update(double deltaTime)
{
    double pressure = parentBody->ComputeAtmosphericPressure(altitudeMeters);
    double gravity = parentBody->ComputeGravitationalAcceleration(altitudeMeters);

    // Thrust
    if (fuelMassKg > 0.0)
    {
        double thrust = engine.ComputeThrust(pressure);
        double acceleration = thrust / GetMass();
        velocityMetersPerSecond += acceleration * deltaTime;

        double massFlowRate = engine.ComputeMassFlowRate(pressure);
        double fuelUsed = massFlowRate * deltaTime;
        fuelMassKg -= std::min(fuelUsed, fuelMassKg);
    }

    // Gravity
    velocityMetersPerSecond -= gravity * deltaTime;

    // Position update
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