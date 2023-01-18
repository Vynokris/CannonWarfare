#pragma once

#pragma region Custom Includes
#include "Particle.h"
#pragma endregion

class ParticlesHelper
{
public:
    ParticlesHelper()  = default;
    ~ParticlesHelper() = default;

    inline static Particle* LINE = new Particle(Particle::Shape::LINE, {2, 2}, {5, 5}, 90.f, 1.f, 2.f, Color{255, 255, 255, 1});
};
