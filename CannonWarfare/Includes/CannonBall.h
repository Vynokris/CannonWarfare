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

	Color color = { 0, 255, 255, 255 };
	float trajectoryAlpha = 0.f;
	float destroyDuration = 1.f, destroyTimer = 1000.f;

public:
	bool showTrajectory = false;

private:
	Maths::Vector2 ComputeDrag() const;
	void UpdateTrajectory();

public:
	CannonBall(const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity, const float& _groundHeight);

	void Update(const float& deltaTime);
	void Draw() const;
	void DrawTrajectory() const;

	void Destroy();
	bool IsDestroying() const { return 0.f < destroyTimer && destroyTimer < destroyDuration; }
	bool IsDestroyed () const { return destroyTimer < 0.f; }
};