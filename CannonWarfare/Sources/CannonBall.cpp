#include "CannonBall.h"
#include "PhysicsConstants.h"

CannonBall::CannonBall(const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity)
	: position(startPosition), velocity(startVelocity), acceleration({ 0, -GRAVITY })
{
}

void CannonBall::Update(const float& deltaTime)
{
	velocity += acceleration * deltaTime;
	position += velocity     * deltaTime;
}

void CannonBall::Draw()
{
}
