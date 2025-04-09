#pragma once

class Parachute
{
public:
    Parachute(double dragArea, double dragCoefficient, double deployAltitude, double maxSupportedMass);

    bool ShouldDeploy(double altitudeMeters, double vesselMass) const;
    double ComputeDragForce(double airDensity, double velocity, double vesselMass) const;

private:
    double deployAltitude; // m
    double dragCoefficient;
    double dragArea;         // m²
    double maxSupportedMass; // kg
};
