#pragma once
#include "CannonBall.h"
#include <vector>

class Cannon
{
private:
	std::vector<CannonBall*> projectiles;

public:
	Maths::Vector2 position;
	float rotation = 0, shootingVelocity = 0;

	Cannon();
	~Cannon();

	void Update(const float& deltaTime);
	void Draw();

	void Shoot();
};