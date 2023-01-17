#pragma once
#include "Vector2.h"

class CannonBall
{
private:
	Maths::Vector2 position;
	Maths::Vector2 velocity;
	Maths::Vector2 acceleration;

public:
	CannonBall(const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity);

	void Update(const float& deltaTime);
	void Draw();
};