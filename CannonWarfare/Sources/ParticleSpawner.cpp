#include "ParticleSpawner.h"
#include "Arithmetic.h"
#include "ParticleManager.h"
#include <iostream>
using namespace Maths;

static float RandFloatInBounds(const float& min, const float& max)
{
    if (max - min <= 0.001f) return min;
    return (float)(rand() % (int)clampAbove(max * 100 - min * 100, 1.f) + (int)(min * 100)) / 100.f;
}

ParticleSpawner::ParticleSpawner(ParticleManager& _particleManager, const int& _spawnRate, const float& _spawnDuration, const SpawnerParticleParams& _params, const Transform2D* _parentTransform)
    : particleManager(_particleManager), spawnRate(_spawnRate), spawnDuration(_spawnDuration), params(_params), parentTransform(_parentTransform)
{
}

void ParticleSpawner::Update(const float& deltaTime)
{
    if(!IsOutdated())
    {
        for (int i = 0; i < spawnRate; i++)
        {
            float minDir = params.minDirection;
            float maxDir = params.maxDirection;

            if (parentTransform)
            {
                minDir += parentTransform->rotation;
                maxDir += parentTransform->rotation;
            }
            
            const float          randAngle     = RandFloatInBounds(minDir, maxDir);
            const Maths::Vector2 randVelocity  = { randAngle, RandFloatInBounds(params.minVelocity, params.maxVelocity), true };
            const float          randRotation  = degToRad(rand() % 360);
            const float          randAngularV  = RandFloatInBounds(params.minAngularV, params.maxAngularV);
            const float          randSize      = RandFloatInBounds(params.minSize,     params.maxSize);
            const float          randFriction  = RandFloatInBounds(params.minFriction, params.maxFriction);
                  Transform2D    randTransform = { params.position, randVelocity, {}, randRotation, randAngularV };

            if (parentTransform)
            {
                randTransform.position = parentTransform->position;
            }
        
            particleManager.AddParticle(new Particle(params.shape, randTransform, randSize, randFriction, params.color));
        }
        
        spawnDuration -= deltaTime;
    }
}
