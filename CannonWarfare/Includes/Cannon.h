#pragma once
#include "CannonBall.h"
#include "raylib.h"
#include <vector>

class Cannon
{
private:
	std::vector<CannonBall*> projectiles;
	const float& groundHeight;

	Maths::Vector2 position;
	float rotation = 0, shootingVelocity = 0;

	Maths::Vector2 landingPosition, controlPoint;
	float airTime = 0, maxHeight = 0, landingDistance = 0;

	Color trajectoryColor      = MAGENTA;
	Color landingDistanceColor = GREEN;
	Color maxHeightColor       = RED;

	void UpdateTrajectory();

public:
	bool automaticRotation = true;

	Cannon(const float& _groundHeight);
	~Cannon();

	void Update(const float& deltaTime);
	void Draw();

	void Shoot();
	void ClearProjectiles();

	void SetPosition(const Maths::Vector2& pos) { position = pos; UpdateTrajectory(); }
	void SetRotation(const float&          rot) { rotation = rot; UpdateTrajectory(); }
	void SetShootingVelocity(const float&  vel) { shootingVelocity = vel; UpdateTrajectory(); }

	Maths::Vector2 GetPosition() { return position; }
	float          GetRotation() { return rotation; }
	float          GetShootingVelocity() { return shootingVelocity; }

	float GetAirTime        () { return airTime;         }
	float GetMaxHeight      () { return maxHeight;       }
	float GetLandingDistance() { return landingDistance; }
};