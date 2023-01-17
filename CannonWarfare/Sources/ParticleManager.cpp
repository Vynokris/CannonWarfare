#pragma once

#include "ParticleManager.h"
#include "RaylibConversions.h"
#include "raylib.h"
using namespace Maths;

ParticleManager::ParticleManager()
{
}

ParticleManager::~ParticleManager()
{
}

void ParticleManager::DrawParticles(float deltaTime)
{
	if (getParticles().size() == 0) return;

	for (size_t i = 0; i < getParticles().size(); i++) {
		particles[i].SetVelocity(particles[i].GetVelocity() * (1 - particles[i].GetFriction() * deltaTime));
		particles[i].SetPosition(particles[i].GetPosition() + particles[i].GetVelocity()	  * deltaTime);
		particles[i].SetSize    (particles[i].GetSize()     * deltaTime						  * 17);
		
		switch (particles[i].GetShape())
		{
		case Particle::Shape::CIRCLE:
			DrawCircleV(
				ToRayVector2(particles[i].GetPosition()),
				particles[i].GetSize(),
				particles[i].GetColor()
			); break;
		case Particle::Shape::LINE:
			DrawLineEx(
				ToRayVector2(particles[i].GetPosition()),
				ToRayVector2(particles[i].GetPosition() - particles[i].GetVelocity().GetNormalized() * particles[i].GetSize()),
				2, 
				particles[i].GetColor()
			); break;
		case Particle::Shape::SQUARE:
			const Rectangle rectangle = Rectangle{
				particles[i].GetPosition().x - particles[i].GetSize() / 2,
				particles[i].GetPosition().y - particles[i].GetSize() / 2,
				particles[i].GetSize(),
				particles[i].GetSize()
			};

			DrawRectanglePro(
				rectangle,
				ToRayVector2(Maths::Vector2()),
				particles[i].GetRotation() * 180 / PI,
				particles[i].GetColor()
			);  break;
		default:
			break;
		}

		if (particles[i].GetSize() <= 0) {
			particles.erase(particles.begin() + i); i--;
		}
	}
}
