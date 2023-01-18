#pragma once

#include "ParticleSpawner.h"
#include <vector>

class ParticleManager
{
private:
	std::vector<ParticleSpawner*> particleSpawners;
	std::vector<Particle*>        particles;

public:
	~ParticleManager();
	
	// Methods
	void Update(const float& deltaTime);
	void Draw() const;
	void CreateSpawner(const int& spawnRate, const float& spawnDuration, const SpawnerParticleParams& params, const Maths::Transform2D* parentTransform = nullptr);
	void AddParticle  (Particle* particle);

	// Native Types - Getter
	std::vector<ParticleSpawner*> GetSpawners()  const { return particleSpawners; }
    std::vector<Particle*>        GetParticles() const { return particles; }
};
