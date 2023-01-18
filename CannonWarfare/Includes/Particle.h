#pragma once

#include "Vector2.h"
#include <raylib.h>

#include "Transform2D.h"

enum class ParticleShapes {
	LINE,
	CIRCLE,
	POLYGON,
};

struct Particle
{
	ParticleShapes shape;
	
	Maths::Transform2D transform;
	float lifetime; // TODO
	float size;
	float friction;
	int   sides;

	Color color;

	Particle(const ParticleShapes& _shape, const Maths::Transform2D& _transform, const float& _size, const float& _friction, const Color& _color);

	void Draw() const;
	void Update(const float& deltaTime);

	bool IsOutdated() const { return size <= 0; }
};
