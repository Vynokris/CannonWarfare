#pragma once
#include "Vector2.h"
#include <raylib.h>
#include <chrono>

class CannonBall
{
private:
	Maths::Vector2 position;
	Maths::Vector2 velocity;
	Maths::Vector2 acceleration;
	float weight = 4.f, radius = 0.05f, elasticity = 0.2f;
	const float& groundHeight;

	bool landed = false;
	Maths::Vector2 startPos, startV;
	Maths::Vector2 endPos,   endV;
	Maths::Vector2 controlPoint;
	std::chrono::system_clock::time_point startTime, endTime;
	float airTime = 0;

	Color color = SKYBLUE;
	float destroyDuration = 1.f, destroyCounter = 1000.f;

	Maths::Vector2 ComputeDrag();

public:
	CannonBall(const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity, const float& _groundHeight);

	void Update(const float& deltaTime);
	void Draw();

	void Destroy();
	bool IsDestroying() { return 0.f < destroyCounter && destroyCounter < destroyDuration; };
	bool IsDestroyed () { return destroyCounter < 0.f; };
};