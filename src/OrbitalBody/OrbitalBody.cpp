#include "OrbitalBody.h"

const double Gravity = 6.67430e-11;

OrbitalBody::OrbitalBody(double m, double r) : mass(m), radius(r) {}

double OrbitalBody::computeGravitationalAcceleration(double altitude) const
{
    double distance = radius + altitude;
    return Gravity * mass / (distance * distance);
}
