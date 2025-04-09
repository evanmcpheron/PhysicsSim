#pragma once

class OrbitalBody
{
public:
    OrbitalBody(double massKg, double radiusMeters);

    double ComputeGravitationalAcceleration(double altitudeMeters) const;

    // New: Pressure at given altitude in Pascals
    double ComputeAtmosphericPressure(double altitudeMeters) const;

private:
    double mass;
    double radius;
};
