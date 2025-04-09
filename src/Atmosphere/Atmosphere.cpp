#include "Atmosphere.h"
#include <cmath>

Atmosphere::Atmosphere(double seaLevelPressure, double seaLevelTemp, double lapseRate, double molarMassAir)
    : seaLevelPressure(seaLevelPressure),
      seaLevelTemp(seaLevelTemp),
      lapseRate(lapseRate),
      molarMassAir(molarMassAir) {}

double Atmosphere::GetTemperature(double altitudeMeters) const
{
    return seaLevelTemp - lapseRate * altitudeMeters;
}

double Atmosphere::GetPressure(double altitudeMeters) const
{
    if (altitudeMeters > 11000.0)
        return 0.0; // troposphere cutoff

    double T = GetTemperature(altitudeMeters);
    double exponent = (9.80665 * molarMassAir) / (gasConstant * lapseRate);
    return seaLevelPressure * std::pow(T / seaLevelTemp, exponent);
}

double Atmosphere::GetDensity(double altitudeMeters) const
{
    double pressure = GetPressure(altitudeMeters);
    double temperature = GetTemperature(altitudeMeters);
    if (temperature <= 0.0)
        return 0.0;
    return (pressure * molarMassAir) / (gasConstant * temperature);
}

double Atmosphere::ComputeDragForce(double altitudeMeters,
                                    double velocity,
                                    double dragCoefficient,
                                    double crossSectionArea) const
{
    double density = GetDensity(altitudeMeters);
    return 0.5 * density * velocity * velocity * dragCoefficient * crossSectionArea;
}
