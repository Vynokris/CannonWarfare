#pragma region Linked Includes
#include "ParticleManager.h"
#pragma endregion

#pragma region Custom Includes
#include "RaylibConversions.h"
#pragma endregion

#pragma region Namespaces
using namespace Maths;
#pragma endregion

void ParticleManager::Update(const float& deltaTime)
{
    /*
     * Execute update logic on all available spawners
     */
    for(size_t i = 0; i < GetSpawners().size(); i++)
    {
        particleSpawners[i]->Update(deltaTime);

        /*
         * State check of all available spawners
         *  If spawner marked as destroy, execute destroy logic
         */
        if(particleSpawners[i]->ShouldDestroy())
        {
            particleSpawners.erase(particleSpawners.begin() + i);
            i--;
        }
    }
}

void ParticleManager::SpawnParticles(Particle* _particle, const int& _spawnRate, const float& _spawnDuration)
{
    particleSpawners.emplace_back(new ParticleSpawner(_particle, _spawnRate, _spawnDuration));
}
