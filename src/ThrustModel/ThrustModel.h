#pragma once

class ThrustModel
{
public:
    // Constructor
    ThrustModel(double maxThrustNewton,
                double specificImpulseVacuum,
                double specificImpulseSeaLevel);

    // Set throttle (0.0 to 1.0)
    void SetThrottle(double newThrottle);

    // Get current throttle
    double GetThrottle() const;

    // Returns the current thrust in Newtons based on ambient pressure
    double ComputeThrust(double ambientPressurePascal) const;

    // Returns the current mass flow rate in kg/s based on ambient pressure
    double ComputeMassFlowRate(double ambientPressurePascal) const;

private:
    double maxThrustNewton;
    double specificImpulseVacuum;
    double specificImpulseSeaLevel;
    double currentThrottle;

    // Constants
    static constexpr double standardGravity = 9.80665; // m/s^2

    // Helper to interpolate ISP based on pressure
    double ComputeCurrentISP(double ambientPressurePascal) const;
};
