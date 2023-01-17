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
		particles[i].setVelocity(particles[i].getVelocity() * (1 - particles[i].getFriction() * deltaTime));
		particles[i].setPosition(particles[i].getPosition() + particles[i].getVelocity()	  * deltaTime);
		particles[i].setSize    (particles[i].getSize()     * deltaTime						  * 17);
		
		switch (particles[i].getShape())
		{
		case Particle::Shape::CIRCLE:
			DrawCircleV(
				ToRayVector2(particles[i].getPosition()),
				particles[i].getSize(),
				particles[i].getColor()
			); break;
		case Particle::Shape::LINE:
			DrawLineEx(
				ToRayVector2(particles[i].getPosition()),
				ToRayVector2(particles[i].getPosition() - particles[i].getVelocity().GetNormalized() * particles[i].getSize()),
				2, 
				particles[i].getColor()
			); break;
		case Particle::Shape::SQUARE:
			const Rectangle rectangle = Rectangle(
				particles[i].getPosition().x - particles[i].getSize() / 2,
				particles[i].getPosition().y - particles[i].getSize() / 2,
				particles[i].getSize(),
				particles[i].getSize()
			);

			DrawRectanglePro(
				rectangle,
				ToRayVector2(Maths::Vector2()),
				particles[i].getRotation() * 180 / PI,
				particles[i].getColor()
			);  break;
		default:
			break;
		}

		if (particles[i].getSize() <= 0) {
			particles.erase(particles.begin() + i); i--;
		}
	}
}
