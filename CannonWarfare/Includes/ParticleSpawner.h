#pragma once

#include "Particle.h"
#include <vector>

class ParticleManager;

struct SpawnerParticleParams
{
    ParticleShapes      shape;
	Maths::Vector2      position;
    float minDirection, maxDirection;
    float minVelocity,  maxVelocity;
    float minLifetime,  maxLifetime;
    float minAngularV,  maxAngularV;
    float minSize,      maxSize;
    float minFriction,  maxFriction;
    Color color;
};

class ParticleSpawner
{
private:
    ParticleManager& particleManager;
    
    int   spawnRate     = 0;
    float spawnDuration = 0;

public:
    SpawnerParticleParams     params;
    const Maths::Transform2D* parentTransform = nullptr;
    
public:
    // Constructor.
    ParticleSpawner(ParticleManager& _particleManager, const int& _spawnRate, const float& _spawnDuration, const SpawnerParticleParams& _params, const Maths::Transform2D* _parentTransform = nullptr);

    // Methods.
    void Update(const float& deltaTime);

    // Getters.
    int   GetSpawnRate()     const { return spawnRate;          }
    float GetSpawnDuration() const { return spawnDuration;      }
    bool  IsOutdated()       const { return spawnDuration <= 0; }
};
