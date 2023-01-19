#include "Particle.h"
#include "RaylibConversions.h"

using namespace Maths;

Particle::Particle(const ParticleShapes& _shape, const Maths::Transform2D& _transform, const float& _size, const float& _friction, const Color& _color)
	: shape(_shape), transform(_transform), size(_size), friction(_friction), color(_color)
{}

void Particle::Draw() const
{
	switch (shape)
	{
	case ParticleShapes::LINE:
	{
		const Maths::Vector2 normalizedV = transform.velocity.GetNormalized();
		DrawLineEx(ToRayVector2(transform.position + normalizedV * 0.5f * size), ToRayVector2(transform.position - normalizedV * 0.5f * size), 1, color);
		break;
	}
	case ParticleShapes::CIRCLE:
		DrawCircleLines((int)transform.position.x, (int)transform.position.y, size, color);
		break;
	case ParticleShapes::POLYGON:
		DrawPolyLines(ToRayVector2(transform.position), 4, size, radToDeg(transform.rotation), color);
		break;
	}
}

void Particle::Update(const float& deltaTime)
{
	transform.acceleration += transform.velocity.GetNegated() * friction * deltaTime;
	transform.Update(deltaTime);
	size -= 100 * deltaTime;
}
