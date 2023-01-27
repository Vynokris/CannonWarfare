#pragma once
#include "Transform2D.h"
#include <raylib.h>
#include <chrono>
#include <vector>

class ParticleManager;

class CannonBall
{
private:
	ParticleManager& particleManager;
	
	Maths::Transform2D transform;
	const float& groundHeight;

	bool landed = false, collided = false;
	Maths::Vector2 startPos, startV;
	Maths::Vector2 endPos,   endV;
	Maths::Vector2 controlPoint;
	std::chrono::system_clock::time_point startTime, endTime;
	float airTime = 0;

	Color color = MAGENTA;
	float trajectoryAlpha = 0.f;
	float destroyDuration = 1.f, destroyTimer = 1.1f;

	std::vector<::Vector2> posHistory; // Used to draw trajectory with drag.

public:
	float radius = 30.f, mass = 4.f, elasticity = 0.25f;
	bool showTrajectory = false;
	bool applyDrag      = false;

private:
	Maths::Vector2 ComputeDrag(const float& deltaTime) const;
	
	void SavePositionToHistory(const bool& forceSave = false);
	void UpdateTrajectory();

	void UpdateColorAlphas (const float& deltaTime);
	void UpdateDestroyTimer(const float& deltaTime);

	void ApplyBouncingLogic();

public:
	CannonBall(ParticleManager& _particleManager, const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity, const float& predictedAirTime, const float& _groundHeight);

	void Update(const float& deltaTime);
	void CheckCollisions(CannonBall* other);
	
	void Draw() const;
	void DrawTrajectory();
	
	void Destroy();
	bool IsDestroying() const { return 0.f < destroyTimer && destroyTimer < destroyDuration; }
	bool IsDestroyed () const { return destroyTimer < 0.f; }

	Maths::Transform2D GetTransform() const { return transform; }
	bool               HasLanded()    const { return landed; }
};
