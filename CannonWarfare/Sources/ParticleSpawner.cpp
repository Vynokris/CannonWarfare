#pragma region Linked Includes
#include "ParticleSpawner.h"
#pragma endregion

ParticleSpawner::ParticleSpawner(Particle* _particle, const int& _spawnRate, const float& _spawnDuration)
    : baseParticle(_particle), spawnRate(_spawnRate), spawnDuration(_spawnDuration)
{}

void ParticleSpawner::Update(const float& deltaTime)
{
    /*
     * State check of particle spawner
     *  If spawner is outdated, stop spawning logic.
    */
    if(!IsOutdated())
    {
        spawnDuration -= deltaTime;
        spawnTimer -= deltaTime;

        /*
         * State check of spawner timer
         *  If timer is ready, execute spawning logic.
         */
        if (CanSpawnParticles())
        {
            for(int i = 0; i < spawnRate; i++)
                particles.emplace_back(new Particle(baseParticle));
            
            ResetSpawnTimer();
        }
    }

    /*
     * Execute update logic on all available particles.
     */
    for (size_t i = 0; i < GetParticles().size(); i++)
    {
        particles[i]->Update(deltaTime);

        /*
         * State check of available particles
         *  If particle is outdated, execute erase logic.
         */
        if (particles[i]->IsOutdated())
        {
            particles.erase(particles.begin() + i);
            i--;
        }
    }
}

void ParticleSpawner::Draw()
{
    /*
     * Execute draw logic on all available particles
     */
    for (Particle* particle : GetParticles())
    {
        particle->Draw();
    }
}
