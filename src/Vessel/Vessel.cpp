#include "Vessel.h"
#include <OrbitalBody.h>

Vessel::Vessel(double startingAltitude,
               double startingVelocity,
               double dryMassKg,
               double fuelMassKg,
               double dragCoefficient,
               double crossSectionArea,
               OrbitalBody *parentBody,
               const ThrustModel &engineModel)
    : altitudeMeters(startingAltitude),
      angleOfAttackRadians(0.0),
      crossSectionArea(crossSectionArea),
      currentHeatRate(0.0),
      dragCoefficient(dragCoefficient),
      dryMassKg(dryMassKg),
      engine(engineModel),
      flightPathAngleRadians(0.0),
      fuelMassKg(fuelMassKg),
      hasBurnedUp(false),
      hasCrashed(false),
      hasDirectionalAerodynamics(true),
      hasLandedSafely(false),
      heatShield(nullptr),
      initialFuelMassKg(fuelMassKg),
      lastAirDensity(0.0),
      lastDragAcceleration(0.0),
      lastDragForce(0.0),
      lastLiftForce(0.0),
      lastLiftVector(0.0, 0.0, 0.0),
      orientationVector(0.0, 1.0, 0.0),
      parentBody(parentBody),
      positionVector(0.0, 0.0, 0.0),
      surfaceTemperature(0.0),
      totalHeatLoad(0.0),
      velocityMetersPerSecond(startingVelocity),
      velocityVector(0.0, 0.0, 0.0)
{
}

// ==============================
// Update per time step
// ==============================
void Vessel::Update(double deltaTime)
{
    double altitude = altitudeMeters;
    double gravity = parentBody->ComputeGravitationalAcceleration(altitude);
    double pressure = parentBody->ComputeAtmosphericPressure(altitude);

    ApplyThrust(deltaTime, pressure);
    ComputeVelocityVector();
    ComputeAngleOfAttack();
    ApplyDrag(deltaTime);
    ApplyLift(deltaTime);
    ApplyParachuteDrag(deltaTime);
    ApplyReentryHeating(deltaTime);
    ApplyHeatShield(deltaTime);
    ApplyRadiativeCooling(deltaTime);

    // === Gravity ===
    velocityMetersPerSecond -= gravity * deltaTime;

    // === Update Altitude ===
    altitudeMeters += velocityMetersPerSecond * deltaTime;

    positionVector = Vector3(0.0, parentBody->GetRadius() + altitudeMeters, 0.0);

    ComputeFlightPathAngle();
    EvaluateReentryOutcome();
}

void Vessel::ApplyThrust(double deltaTime, double pressure)
{
    if (fuelMassKg <= 0.0)
        return;

    double thrust = engine.ComputeThrust(pressure);
    double acceleration = thrust / GetMass();
    velocityMetersPerSecond += acceleration * deltaTime;

    double massFlowRate = engine.ComputeMassFlowRate(pressure);
    double fuelUsed = massFlowRate * deltaTime;
    fuelMassKg -= std::min(fuelUsed, fuelMassKg);
}

void Vessel::ComputeVelocityVector()
{
    velocityVector = Vector3(0.0, velocityMetersPerSecond, 0.0); // Extend to full 3D later
}

void Vessel::ComputeAngleOfAttack()
{
    if (hasDirectionalAerodynamics)
        angleOfAttackRadians = velocityVector.AngleBetween(orientationVector);
    else
        angleOfAttackRadians = 0.0;
}

void Vessel::ApplyParachuteDrag(double deltaTime)
{
    if (parachute && !parachuteDeployed && parachute->ShouldDeploy(altitudeMeters, GetMass()))
    {
        parachuteDeployed = true;
        std::cout << "🪂 Parachute deployed at " << altitudeMeters << " m\n";
    }

    if (parachuteDeployed && parentBody->GetAtmosphere())
    {
        double chuteDrag = parachute->ComputeDragForce(lastAirDensity, velocityMetersPerSecond, GetMass());
        double chuteAccel = chuteDrag / GetMass();
        double chuteDir = (velocityMetersPerSecond > 0.0) ? -1.0 : 1.0;
        velocityMetersPerSecond += chuteDir * chuteAccel * deltaTime;
    }
}

void Vessel::EvaluateReentryOutcome()
{
    if (altitudeMeters > 0.0)
        return;

    double impactSpeed = std::abs(velocityMetersPerSecond);

    // Burnup condition: no shield + high heat rate or temp
    if (IsHeatShieldDepleted() && (currentHeatRate > 20000.0 || surfaceTemperature > 1200.0))
    {
        hasBurnedUp = true;
        return;
    }

    // Crash condition: high impact speed
    if (impactSpeed > 15.0)
    {
        hasCrashed = true;
        return;
    }

    // Otherwise, it's safe
    hasLandedSafely = true;
}

void Vessel::ComputeFlightPathAngle()
{
    Vector3 rHat = positionVector.Normalized();
    Vector3 vHat = velocityVector.Normalized();

    double dot = vHat.Dot(rHat);             // how aligned with "up" are we?
    dot = std::clamp(dot, -1.0, 1.0);        // for safety
    flightPathAngleRadians = std::asin(dot); // returns [-π/2, π/2]
}

void Vessel::ApplyDrag(double deltaTime)
{
    lastAirDensity = 0.0;
    lastDragForce = 0.0;
    lastDragAcceleration = 0.0;

    if (Atmosphere *atm = parentBody->GetAtmosphere())
    {
        lastAirDensity = atm->GetDensity(altitudeMeters);
        double aoaModifier = hasDirectionalAerodynamics ? std::abs(std::cos(angleOfAttackRadians)) : 1.0;
        double effectiveCd = dragCoefficient * aoaModifier;

        lastDragForce = atm->ComputeDragForce(
            altitudeMeters,
            velocityMetersPerSecond,
            effectiveCd,
            crossSectionArea);

        lastDragAcceleration = lastDragForce / GetMass();
        double dragDirection = (velocityMetersPerSecond > 0.0) ? -1.0 : 1.0;
        velocityMetersPerSecond += dragDirection * lastDragAcceleration * deltaTime;
    }
}

void Vessel::ApplyLift(double deltaTime)
{
    lastLiftForce = 0.0;
    lastLiftVector = Vector3(0.0, 0.0, 0.0);

    Atmosphere *atm = parentBody->GetAtmosphere();
    if (atm && hasDirectionalAerodynamics)
    {
        double rho = lastAirDensity;
        double v = velocityVector.Length();

        double cl = 2.0 * M_PI * std::sin(angleOfAttackRadians);
        cl = std::clamp(cl, -1.5, 1.5);

        double liftForceMagnitude = 0.5 * rho * v * v * cl * crossSectionArea;
        lastLiftForce = liftForceMagnitude;

        Vector3 vDir = velocityVector.Normalized();
        Vector3 reference(0.0, 0.0, 1.0); // World forward

        // Avoid degenerate cross when vectors are parallel
        if (std::abs(vDir.Dot(reference)) > 0.99)
            reference = Vector3(1.0, 0.0, 0.0);

        Vector3 liftDir = vDir.Cross(reference).Normalized().Cross(vDir).Normalized();

        lastLiftVector = liftDir * liftForceMagnitude;

        velocityMetersPerSecond += (lastLiftVector.y / GetMass()) * deltaTime;
    }
}

void Vessel::ApplyReentryHeating(double deltaTime)
{
    if (!parentBody->GetAtmosphere())
    {
        currentHeatRate = 0.0;
        return;
    }

    double rho = lastAirDensity;
    double velocity = std::abs(velocityMetersPerSecond);
    constexpr double heatTransferCoefficient = 1.83e-4;

    currentHeatRate = heatTransferCoefficient * rho * velocity * velocity * velocity;

    double aoaModifier = hasDirectionalAerodynamics ? std::abs(std::cos(angleOfAttackRadians)) : 1.0;
    currentHeatRate *= aoaModifier;

    totalHeatLoad += currentHeatRate * deltaTime;
}

void Vessel::AttachParachute(Parachute *p)
{
    parachute = p;
    parachuteDeployed = false;
}

void Vessel::ApplyHeatShield(double deltaTime)
{
    if (heatShield && !heatShield->IsDepleted())
        heatShield->AbsorbHeat(currentHeatRate, deltaTime);
}

void Vessel::ApplyRadiativeCooling(double deltaTime)
{
    constexpr double emissivity = 0.85;
    constexpr double stefanBoltzmann = 5.670374419e-8;
    constexpr double heatCapacityPerArea = 2000.0;

    surfaceTemperature = std::pow(
        std::max(0.0, totalHeatLoad) / heatCapacityPerArea,
        1.0);

    double radiatedPower = emissivity * stefanBoltzmann * std::pow(surfaceTemperature, 4.0);
    totalHeatLoad = std::max(0.0, totalHeatLoad - radiatedPower * deltaTime);
}

// ==============================
// Attach heat shield
// ==============================
void Vessel::AttachHeatShield(HeatShield *shield)
{
    heatShield = shield;
}

// ==============================
// Setters
// ==============================
void Vessel::SetThrottle(double throttle)
{
    engine.SetThrottle(throttle);
}

void Vessel::SetOrientationVector(const Vector3 &orientation)
{
    orientationVector = orientation.Normalized();
}

// ==============================
// Getters
// ==============================
double Vessel::GetAltitude() const { return altitudeMeters; }
double Vessel::GetVelocity() const { return velocityMetersPerSecond; }
double Vessel::GetMass() const { return dryMassKg + fuelMassKg; }
double Vessel::GetFuelMass() const { return fuelMassKg; }
bool Vessel::IsParachuteDeployed() const
{
    return parachuteDeployed;
}
double Vessel::GetFuelPercent() const
{
    if (initialFuelMassKg <= 0.0)
        return 0.0;
    return (fuelMassKg / initialFuelMassKg) * 100.0;
}
double Vessel::GetHeatRate() const { return currentHeatRate; }
double Vessel::GetTotalHeatLoad() const { return totalHeatLoad; }
double Vessel::GetHeatShieldMass() const
{
    return heatShield ? heatShield->GetRemainingMass() : 0.0;
}
double Vessel::GetAblatedMass() const
{
    return heatShield ? heatShield->GetTotalAblatedMass() : 0.0;
}
double Vessel::GetHeatShieldSurfaceTemp() const
{
    return heatShield ? heatShield->GetSurfaceTemperature() : 0.0;
}
bool Vessel::IsHeatShieldDepleted() const
{
    return heatShield ? heatShield->IsDepleted() : true;
}
double Vessel::GetAngleOfAttackDegrees() const
{
    return angleOfAttackRadians * (180.0 / M_PI);
}

Vector3 Vessel::GetLiftVector() const
{
    return lastLiftVector;
}

double Vessel::GetLiftForce() const
{
    return lastLiftForce;
}

double Vessel::GetFlightPathAngleDegrees() const
{
    return flightPathAngleRadians * (180.0 / M_PI);
}

bool Vessel::HasBurnedUp() const
{
    return hasBurnedUp;
}

bool Vessel::HasCrashed() const
{
    return hasCrashed;
}

bool Vessel::HasLandedSafely() const
{
    return hasLandedSafely;
}
