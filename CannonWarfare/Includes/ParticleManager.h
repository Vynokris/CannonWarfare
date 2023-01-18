#pragma once

#pragma region Native Includes
#include <vector>
#pragma endregion

#pragma region Custom Includes
#include "ParticleSpawner.h"
#pragma endregion

class ParticleManager
{
private:
	std::vector<ParticleSpawner*> particleSpawners;

public:
	// Constructor & Destructor
	ParticleManager()  {}
	~ParticleManager() {}

	// Methods
	void Update(const float& deltaTime);
	void SpawnParticles(Particle* _particle, const int& _spawnRate, const float& _spawnDuration);

	// Native Types - Getter
	std::vector<ParticleSpawner*> GetSpawners() { return particleSpawners; }
};
