#include "ThrustModel.h"
#include <algorithm> // for std::clamp

ThrustModel::ThrustModel(double maxThrustNewton,
                         double specificImpulseVacuum,
                         double specificImpulseSeaLevel)
    : maxThrustNewton(maxThrustNewton),
      specificImpulseVacuum(specificImpulseVacuum),
      specificImpulseSeaLevel(specificImpulseSeaLevel),
      currentThrottle(0.0) {}

void ThrustModel::SetThrottle(double newThrottle)
{
    currentThrottle = std::clamp(newThrottle, 0.0, 1.0);
}

double ThrustModel::GetThrottle() const
{
    return currentThrottle;
}

double ThrustModel::ComputeCurrentISP(double ambientPressurePascal) const
{
    // Define sea-level pressure for scaling
    constexpr double seaLevelPressure = 101325.0; // Pa

    // Clamp pressure between vacuum and sea level
    double clampedPressure = std::clamp(ambientPressurePascal, 0.0, seaLevelPressure);
    double pressureRatio = clampedPressure / seaLevelPressure;

    // Linear interpolation between sea level ISP and vacuum ISP
    return specificImpulseSeaLevel + (specificImpulseVacuum - specificImpulseSeaLevel) * (1.0 - pressureRatio);
}

double ThrustModel::ComputeThrust(double ambientPressurePascal) const
{
    double currentISP = ComputeCurrentISP(ambientPressurePascal);

    // Adjust thrust linearly based on ISP (optional: some engines vary thrust with pressure)
    double thrust = maxThrustNewton * (currentISP / specificImpulseVacuum);
    return thrust * currentThrottle;
}

double ThrustModel::ComputeMassFlowRate(double ambientPressurePascal) const
{
    double currentISP = ComputeCurrentISP(ambientPressurePascal);
    double thrust = ComputeThrust(ambientPressurePascal);

    // ṁ = F / (Isp * g0)
    return thrust / (currentISP * standardGravity);
}
