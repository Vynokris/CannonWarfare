#pragma once

#pragma region Native Includes
#include <vector>
#pragma endregion

#pragma region Custom Includes
#include "Particle.h"
#pragma endregion

class ParticleSpawner
{
private:
    // No interactions with code flow
    float     spawnTimer = 0;
    Particle* baseParticle   = nullptr;
    
private:
    // Interactions with code flow
    int   spawnRate     = 0;
    float spawnDuration = 0;
    
    std::vector<Particle*> particles;
    
public:
    // Constructor & Destructor
    ParticleSpawner(Particle* _particle, const int& _spawnRate, const float& _spawnDuration);
    ~ParticleSpawner() = default;

    // Methods
    void Update(const float& deltaTime);
    void Draw();
    void ResetSpawnTimer() { spawnTimer = 1.f / (float)GetSpawnRate(); }

    // Native Types - Getters
    int   GetSpawnRate()      const { return spawnRate;          }
    float GetSpawnDuration()  const { return spawnDuration;      }
    bool  IsOutdated()        const { return spawnDuration <= 0; }
    bool  CanSpawnParticles() const { return spawnTimer    <= 0; }
    bool  ShouldDestroy()     const { return IsOutdated() && GetParticles().empty(); }
    
    std::vector<Particle*> GetParticles() const { return particles; }
    
    // Custom Types - Getters
    Particle* GetBaseParticle()  const { return baseParticle; }
};
