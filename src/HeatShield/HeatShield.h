#pragma once

class HeatShield
{
public:
    HeatShield(double massKg,
               double areaM2,
               double ablationEnergyJPerKg,
               double maxTempK = 2500.0);

    // Apply incoming heat (W/m²) over a timestep
    void AbsorbHeat(double heatFluxWPerM2, double deltaTime);

    double GetRemainingMass() const;
    double GetTotalAblatedMass() const;
    double GetSurfaceTemperature() const;

    bool IsDepleted() const;

private:
    double ablationEnergyPerKg; // J/kg
    double area;                // m² (shielded)
    double initialMass;
    double mass;             // kg (remaining)
    double maxSurfaceTemp;   // K
    double surfaceTemp;      // K
    double totalAblatedMass; // kg
};
