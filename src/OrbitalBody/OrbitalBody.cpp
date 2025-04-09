#include "OrbitalBody.h"
#include <cmath>

const double Gravity = 6.67430e-11;

OrbitalBody::OrbitalBody(double m, double r) : mass(m), radius(r) {}

double OrbitalBody::ComputeGravitationalAcceleration(double altitude) const
{
    double distance = radius + altitude;
    return Gravity * mass / (distance * distance);
}

double OrbitalBody::ComputeAtmosphericPressure(double altitudeMeters) const
{
    // Constants for Earth's troposphere (0–11 km)
    constexpr double seaLevelPressure = 101325.0; // Pa
    constexpr double lapseRate = 0.0065;          // K/m
    constexpr double seaLevelTemp = 288.15;       // K
    constexpr double gasConstant = 8.3144598;     // J/(mol·K)
    constexpr double molarMassAir = 0.0289644;    // kg/mol
    constexpr double gravity = 9.80665;           // m/s²

    if (altitudeMeters > 11000.0)
        return 0.0; // no pressure above 11km (simplified)

    double exponent = (gravity * molarMassAir) / (gasConstant * lapseRate);
    double term = 1.0 - ((lapseRate * altitudeMeters) / seaLevelTemp);
    return seaLevelPressure * std::pow(term, exponent);
}
