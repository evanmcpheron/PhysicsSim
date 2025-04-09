#pragma once
#include <Atmosphere.h>

class OrbitalBody
{
public:
    OrbitalBody(double massKg, double radiusMeters, Atmosphere *atmosphere = nullptr);

    double ComputeGravitationalAcceleration(double altitudeMeters) const;

    double ComputeAtmosphericPressure(double altitudeMeters) const;

    Atmosphere *GetAtmosphere() const;

    double GetRadius() const;

private:
    double mass;
    double radius;
    Atmosphere *atmosphere;
};
