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
	ParticleManager()  = default;
	~ParticleManager() = default;

	// Methods
	void Update(const float& deltaTime);
	void Draw();
	void SpawnParticles(Particle* _particle, const int& _spawnRate, const float& _spawnDuration);

	// Native Types - Getter
	std::vector<ParticleSpawner*> GetSpawners() { return particleSpawners; }
};
