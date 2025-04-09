#include "HeatShield.h"
#include <algorithm>

HeatShield::HeatShield(double massKg,
                       double areaM2,
                       double ablationEnergyJPerKg,
                       double maxTempK)
    : ablationEnergyPerKg(ablationEnergyJPerKg),
      area(areaM2),
      initialMass(massKg),
      mass(massKg),
      maxSurfaceTemp(maxTempK),
      surfaceTemp(0.0),
      totalAblatedMass(0.0) {}

void HeatShield::AbsorbHeat(double heatFluxWPerM2, double deltaTime)
{
    if (mass <= 0.0)
        return;

    // Total heat energy applied (J)
    double energy = heatFluxWPerM2 * area * deltaTime;

    // Mass that would be ablated by this heat
    double massToAblate = energy / ablationEnergyPerKg;

    // Limit to what's available
    double actualAblated = std::min(massToAblate, mass);

    mass -= actualAblated;
    totalAblatedMass += actualAblated;

    // Estimate temp (optional - can link to heat flux instead)
    surfaceTemp = (mass > 0.0)
                      ? maxSurfaceTemp * (1.0 - mass / initialMass)
                      : maxSurfaceTemp;
}

double HeatShield::GetRemainingMass() const
{
    return mass;
}

double HeatShield::GetTotalAblatedMass() const
{
    return totalAblatedMass;
}

double HeatShield::GetSurfaceTemperature() const
{
    return surfaceTemp;
}

bool HeatShield::IsDepleted() const
{
    return mass <= 0.0;
}
