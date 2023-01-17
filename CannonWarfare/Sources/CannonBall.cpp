#include "CannonBall.h"
#include "PhysicsConstants.h"
#include "raylib.h"

CannonBall::CannonBall(const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity, const float& _groundHeight)
	: position(startPosition), velocity(startVelocity), acceleration({ 0, GRAVITY * 50 }), groundHeight(_groundHeight)
{
}

void CannonBall::Update(const float& deltaTime)
{
	if (position.y < groundHeight - 30)
	{
		velocity += acceleration * deltaTime;
		position += velocity     * deltaTime;
	}
	else if (position.y > groundHeight - 30)
	{
		if (velocity.GetLength() <= 10)
		{
			position.y   = groundHeight - 30;
			velocity     = {};
			acceleration = {};
		}
		else
		{
			position.y  = groundHeight - 30.01f;
			velocity.y *= -1;
			velocity.SetLength(velocity.GetLength() * elasticity);
		}
	}
}

void CannonBall::Draw()
{
	DrawCircleLines(position.x, position.y, 30, WHITE);
}
