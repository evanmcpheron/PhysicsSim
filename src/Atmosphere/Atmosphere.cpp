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
    // Constants
    constexpr double g = 9.80665;     // m/s²
    constexpr double R = gasConstant; // J/mol·K

    if (altitudeMeters <= 11000.0)
    {
        // Troposphere model with lapse rate
        double T = GetTemperature(altitudeMeters);
        double exponent = (g * molarMassAir) / (R * lapseRate);
        return seaLevelPressure * std::pow(T / seaLevelTemp, exponent);
    }
    else
    {
        // Exponential falloff above 11 km
        constexpr double scaleHeight = 7000.0; // Approximate for Earth
        return seaLevelPressure * std::exp(-altitudeMeters / scaleHeight);
    }
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
