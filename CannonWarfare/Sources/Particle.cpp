#pragma region Linked Includes
#include "Particle.h"
#pragma endregion

#pragma region Custom Includes
#include "RaylibConversions.h"
#pragma endregion

#pragma region Namespaces
using namespace Maths;
#pragma endregion

Particle::Particle(const Shape& _shape, const Maths::Vector2& _position, const Maths::Vector2& _velocity, const float& _rotation, const float& _size, const float& _friction, const Color& _color)
	: shape(_shape), position(_position), velocity(_velocity), rotation(_rotation), size(_size), friction(_friction), color(_color)
{}

Particle::Particle(const Particle* particle)
	: shape(particle->GetShape()), position(particle->GetPosition()), velocity(particle->GetVelocity()), rotation(particle->GetRotation()),
	  size(particle->GetSize()), friction(particle->GetFriction()), color(particle->GetColor())
{}

void Particle::Draw()
{
	switch (GetShape())
	{
	case Shape::CIRCLE:
		DrawCircleV(ToRayVector2(GetPosition()), GetSize(), GetColor());
		break;
	case Shape::LINE:
		DrawLineEx(ToRayVector2(GetPosition()), ToRayVector2(GetPosition() - GetVelocity().GetNormalized() * GetSize()), 2, GetColor());
		break;
	case Shape::SQUARE:
		const Rectangle rectangle = Rectangle {
			GetPosition().x - GetSize() / 2,
			GetPosition().y - GetSize() / 2,
			GetSize(),
			GetSize()
		};

		DrawRectanglePro(rectangle, ToRayVector2(Maths::Vector2()), GetRotation() * 180 / PI, GetColor());
		break;
	}
}

void Particle::Update(const float& deltaTime)
{
	SetVelocity(GetVelocity() * (1 - GetFriction() * deltaTime));
	SetPosition(GetPosition() + GetVelocity()	   * deltaTime);
	SetSize    (GetSize()     * deltaTime		   * 17);
}
