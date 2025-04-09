#pragma once

class Atmosphere
{
public:
    Atmosphere(double seaLevelPressure, double seaLevelTemp, double lapseRate,
               double molarMassAir = 0.0289644);

    double GetPressure(double altitudeMeters) const;
    double GetTemperature(double altitudeMeters) const;
    double GetDensity(double altitudeMeters) const;

    double ComputeDragForce(double altitudeMeters,
                            double velocity,
                            double dragCoefficient,
                            double crossSectionArea) const;

private:
    double seaLevelPressure; // in Pascals
    double seaLevelTemp;     // in Kelvin
    double lapseRate;        // in K/m
    double molarMassAir;     // in kg/mol

    // Constants
    static constexpr double gasConstant = 8.3144598; // J/(mol·K)
};
