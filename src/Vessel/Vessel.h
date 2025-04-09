#pragma once

#include <iostream>
#include <HeatShield.h>
#include <ThrustModel.h>
#include <Vector3.h>
#include <Parachute.h>

class OrbitalBody; // forward declare to avoid circular include

class Vessel
{
public:
    Vessel(double startingAltitude,
           double startingVelocity,
           double dryMassKg,
           double fuelMassKg,
           double dragCoefficient,
           double crossSectionArea,
           OrbitalBody *parentBody,
           const ThrustModel &engineModel);

    void Update(double deltaTime);

    void ApplyThrust(double deltaTime, double pressure);

    void SetThrottle(double throttle);

    double GetAltitude() const;
    double GetVelocity() const;
    double GetMass() const;
    double GetFuelMass() const;
    double GetFuelPercent() const;

    double GetLastAirDensity() const { return lastAirDensity; }
    double GetLastDragForce() const { return lastDragForce; }
    double GetLastDragAcceleration() const { return lastDragAcceleration; }
    double GetSurfaceTemperature() const { return surfaceTemperature; }
    double GetHeatShieldMass() const;
    double GetHeatShieldSurfaceTemp() const;
    double GetAblatedMass() const;
    bool IsHeatShieldDepleted() const;
    double GetAngleOfAttackDegrees() const;
    void ComputeVelocityVector();
    void ComputeAngleOfAttack();
    void EvaluateReentryOutcome();
    void ApplyDrag(double deltaTime);
    void ApplyLift(double deltaTime);
    void ApplyReentryHeating(double deltaTime);
    void ApplyHeatShield(double deltaTime);
    void ApplyRadiativeCooling(double deltaTime);
    void AttachHeatShield(HeatShield *shield);
    Vector3 GetLiftVector() const;
    double GetLiftForce() const;
    void SetOrientationVector(const Vector3 &orientation);
    void ComputeFlightPathAngle();
    double GetFlightPathAngleDegrees() const;
    bool HasBurnedUp() const;
    bool HasCrashed() const;
    bool HasLandedSafely() const;
    void AttachParachute(Parachute *p);
    bool IsParachuteDeployed() const;
    void ApplyParachuteDrag(double deltaTime);

    double GetHeatRate() const;
    double GetTotalHeatLoad() const;

private:
    double altitudeMeters;
    double angleOfAttackRadians;
    double crossSectionArea;
    double currentHeatRate; // W/m²
    double dragCoefficient;
    double dryMassKg;
    ThrustModel engine;
    double flightPathAngleRadians;
    double fuelMassKg;
    bool hasBurnedUp = false;
    bool hasCrashed = false;
    bool hasDirectionalAerodynamics; // false for sphere, true for cone/cylinder
    bool hasLandedSafely = false;
    HeatShield *heatShield;
    double initialFuelMassKg;
    double lastAirDensity;
    double lastDragAcceleration;
    double lastDragForce;
    double lastLiftForce;      // N
    Vector3 lastLiftVector;    // for logging or debugging
    Vector3 orientationVector; // ship’s pointing direction
    Parachute *parachute;
    bool parachuteDeployed;
    OrbitalBody *parentBody;
    Vector3 positionVector;
    double surfaceTemperature; // K
    double totalHeatLoad;      // J/m²
    double velocityMetersPerSecond;
    Vector3 velocityVector; // live velocity
};
