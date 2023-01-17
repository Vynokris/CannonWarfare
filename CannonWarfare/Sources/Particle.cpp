#include "Particle.h"
using namespace Maths;

Particle::Particle(const Shape& _shape, const Maths::Vector2& _position, const Maths::Vector2& _velocity, const float& _rotation, const float& _size, const float& _acceleration, const float& _friction, const Color& _color)
	: shape(_shape), position(_position), velocity(_velocity), rotation(_rotation), size(_size), acceleration(_acceleration), friction(_friction), color(_color)
{
}

Particle::~Particle()
{
}
