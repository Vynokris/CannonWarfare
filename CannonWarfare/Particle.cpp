#include "Particle.h"
using namespace Maths;

Particle::Particle(const Maths::Vector2& _position, const float& _rotation, const float& _size, const Color& _color, const Maths::Vector2& _velocity, const float& _acceleration, const float& _friction, const Shape& _shape)
	: position(_position), rotation(_rotation), size(_size), color(_color), velocity(_velocity), acceleration(_acceleration), friction(_friction), shape(_shape)
{
}

Particle::~Particle()
{
}
