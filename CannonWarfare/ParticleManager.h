#pragma once
#include <vector>
#include "Particle.h"

class ParticleManager
{
private:
	std::vector<Particle> particles;

public:
	ParticleManager();
	~ParticleManager();

	void DrawParticles(float deltaTime);

	void AddParticles(const Particle& particle) { particles.emplace_back(particle); }

	std::vector<Particle> getParticles() { return particles; }
};