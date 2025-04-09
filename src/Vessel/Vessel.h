#pragma once

struct Vessel
{
    double startingAltitude; // m
    double startingVelocity; // m/s (vertical)
    double vesselMass;       // kg

    Vessel(double startingAltitude, double startingVelocity, double vesselMass);

    void applyForces(double gravitationalAcceleration, double deltaTime);
};
