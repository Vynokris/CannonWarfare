#pragma once
#include "Vector2.h"

class CannonBall
{
private:
	Maths::Vector2 position;
	Maths::Vector2 velocity;
	Maths::Vector2 acceleration;
	float elasticity = 0.2f;
	const float& groundHeight;

public:
	CannonBall(const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity, const float& _groundHeight);

	void Update(const float& deltaTime);
	void Draw();
};