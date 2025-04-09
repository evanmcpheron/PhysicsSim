#include "OrbitalBody.h"

const double Gravity = 6.67430e-11;

OrbitalBody::OrbitalBody(double m, double r, Atmosphere *a)
    : mass(m), radius(r), atmosphere(a) {}

double OrbitalBody::ComputeGravitationalAcceleration(double altitude) const
{
    double distance = radius + altitude;
    return Gravity * mass / (distance * distance);
}

double OrbitalBody::ComputeAtmosphericPressure(double altitude) const
{
    if (atmosphere)
        return atmosphere->GetPressure(altitude);
    return 0.0;
}

Atmosphere *OrbitalBody::GetAtmosphere() const
{
    return atmosphere;
}

double OrbitalBody::GetRadius() const
{
    return radius;
}