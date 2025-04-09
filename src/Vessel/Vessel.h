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
           OrbitalBody *parentBody,
           const ThrustModel &engineModel); // new

    void Update(double deltaTime); // NEW combined step: gravity + thrust

    void SetThrottle(double throttle);

    double GetAltitude() const;
    double GetVelocity() const;
    double GetMass() const;
    double GetFuelMass() const;
    double GetFuelPercent() const;

private:
    double altitudeMeters;
    double velocityMetersPerSecond;
    double dryMassKg;
    double fuelMassKg;
    double initialFuelMassKg;

    OrbitalBody *parentBody;

    ThrustModel engine;
};
