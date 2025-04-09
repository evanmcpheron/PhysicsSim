#include <iostream>
#include "OrbitalBody.h"
#include "Vessel.h"

int main()
{
    OrbitalBody planet(5.972e24, 6.371e6); // Earth
    Vessel rocket(500.0, 1000.0, 0.0);     // Mass, altitude, initial velocity

    double time = 0.0;
    double deltaTime = 1.0;

    while (rocket.startingAltitude > 0)
    {
        double gravity = planet.computeGravitationalAcceleration(rocket.startingAltitude);
        rocket.applyForces(gravity, deltaTime);

        std::cout << "Time: " << time
                  << "s, Altitude: " << rocket.startingAltitude
                  << "m, Velocity: " << rocket.startingVelocity << " m/s\n";

        time += deltaTime;
    }

    std::cout << "Rocket crashed after " << time << " seconds.\n";
    return 0;
}
