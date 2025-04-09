#include "Parachute.h"
#include <algorithm>

Parachute::Parachute(double dragArea, double dragCoefficient, double deployAltitude, double maxSupportedMass)
    : deployAltitude(deployAltitude),
      dragCoefficient(dragCoefficient),
      dragArea(dragArea),
      maxSupportedMass(maxSupportedMass)
{
}

bool Parachute::ShouldDeploy(double altitudeMeters, double vesselMass) const
{
    return altitudeMeters <= deployAltitude && vesselMass <= maxSupportedMass;
}

double Parachute::ComputeDragForce(double airDensity, double velocity, double vesselMass) const
{
    if (vesselMass > maxSupportedMass)
        return 0.0; // chute fails or won't open

    double v2 = velocity * velocity;
    return 0.5 * airDensity * v2 * dragCoefficient * dragArea;
}
