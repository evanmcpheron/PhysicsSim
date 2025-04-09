#pragma once

#include "../ThrustModel/ThrustModel.h"
#include <iostream>

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

    double GetHeatRate() const;
    double GetTotalHeatLoad() const;

private:
    double altitudeMeters;
    double velocityMetersPerSecond;
    double dryMassKg;
    double fuelMassKg;
    double initialFuelMassKg;
    double dragCoefficient;
    double crossSectionArea;
    OrbitalBody *parentBody;
    ThrustModel engine;
    double totalHeatLoad;            // J/m²
    double currentHeatRate;          // W/m²
    double surfaceTemperature = 0.0; // K

    double lastAirDensity;
    double lastDragForce;
    double lastDragAcceleration;
};
