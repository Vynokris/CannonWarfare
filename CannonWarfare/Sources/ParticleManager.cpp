#include "ParticleManager.h"
#include "RaylibConversions.h"
using namespace Maths;

ParticleManager::~ParticleManager()
{
    for (const Particle* particle : particles)
        delete particle;
}

void ParticleManager::Update(const float& deltaTime)
{
    // Update particle spawners.
    for(size_t i = 0; i < particleSpawners.size(); i++)
    {
        particleSpawners[i]->Update(deltaTime);

        // Erase any outdated spawner.
        if(particleSpawners[i]->IsOutdated())
        {
            particleSpawners.erase(particleSpawners.begin() + i);
            i--;
        }
    }

    // Update particles.
    for (size_t i = 0; i < particles.size(); i++)
    {
        particles[i]->Update(deltaTime);

        // Erase any outdated particle
        if (particles[i]->IsOutdated())
        {
            particles.erase(particles.begin() + i);
            i--;
        }
    }
}

void ParticleManager::Draw() const
{
    for (const Particle* particle : particles)
        particle->Draw();
}

void ParticleManager::CreateSpawner(const int& spawnRate, const float& spawnDuration, const SpawnerParticleParams& params, const Transform2D* parentTransform)
{
    particleSpawners.push_back(new ParticleSpawner(*this, spawnRate, spawnDuration, params, parentTransform));
}

void ParticleManager::AddParticle(Particle* particle)
{
    particles.push_back(particle);
}
