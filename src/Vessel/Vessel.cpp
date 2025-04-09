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
      engine(engineModel) {}

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