#pragma once
#include <cmath>
#include <vector>

class OrbitalBody {
public:
    double mass;      // kg
    double radius;    // meters

    OrbitalBody(double mass, double radius);

    double computeGravitationalAcceleration(double altitude) const;
};
