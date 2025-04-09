#include "Vessel.h"

Vessel::Vessel(double startingAltitude, double startingVelocity, double vesselMass)
    : startingAltitude(startingAltitude), startingVelocity(startingVelocity), vesselMass(vesselMass) {}

void Vessel::applyForces(double gravitationalAcceleration, double deltaTime)
{
    startingVelocity -= gravitationalAcceleration * deltaTime;
    startingAltitude += startingVelocity * deltaTime;
}
