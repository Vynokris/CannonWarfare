#pragma once
#include "CannonBall.h"
#include "raylib.h"
#include <vector>

constexpr int MAX_PROJECTILES = 500;

class ParticleManager;

struct CannonDrawParams
{
	// Cannon colors.
	Color cannonColor          = { 0, 255, 255, 255 };
	Color trajectoryColor      = { 0, 255, 255, 255 };
	Color landingDistanceColor = GREEN;
	Color maxHeightColor       = RED;
	float trajectoryAlpha      = 1.f;
	float measurementsAlpha    = 1.f;

	// Cannon points.
	Maths::Vector2 centerUp;
	Maths::Vector2 centerDown;
	Maths::Vector2 midUp;
	Maths::Vector2 midDown;
	Maths::Vector2 frontUp;
	Maths::Vector2 frontDown;
	Maths::Vector2 wickPointOffset;   
	Maths::Vector2 wickControlOffset;
	Maths::Vector2 wick0;
	Maths::Vector2 wick1;
	Maths::Vector2 wick2;
	Maths::Vector2 wick3;
};

class Cannon
{
private:
	ParticleManager& particleManager;
	std::vector<CannonBall*> projectiles;
	const float& groundHeight;

	// Cannon properties.
	Maths::Vector2 position, shootingPoint;
	float rotation = 0, shootingVelocity = 0;

	// Predicted values for cannonballs.
	Maths::Vector2 landingVelocity, landingPosition, controlPoint, highestPoint;
	float airTime = 0, maxHeight = 0, landingDistance = 0;

	CannonDrawParams drawParams;

public:
	bool automaticRotation = true;
	bool applyDrag         = false;
	bool applyCollisions   = false;
	bool showTrajectory    = true;
	bool showMeasurements  = true;
	bool showProjectileTrajectories = true;
	
private:
	void UpdateTrajectory();
	void UpdateCollisions(CannonBall* cannonBall) const;
	void UpdateDrawPoints();

public:
	Cannon(ParticleManager& _particleManager, const float& _groundHeight);
	~Cannon();

	void Update(const float& deltaTime);
	void Draw() const;
	void DrawTrajectories() const;
	void DrawMeasurements() const;

	void Shoot();
	void ClearProjectiles() const;

	void SetPosition(const Maths::Vector2& pos) { position = pos; UpdateTrajectory(); UpdateDrawPoints(); }
	void SetRotation(const float&          rot) { rotation = rot; UpdateTrajectory(); UpdateDrawPoints(); }
	void SetShootingVelocity(const float&  vel) { shootingVelocity = vel; UpdateTrajectory(); }

	Maths::Vector2 GetPosition()         const { return position; }
	float          GetRotation()         const { return rotation; }
	float          GetShootingVelocity() const { return shootingVelocity; }

	float GetAirTime()         const { return airTime;         }
	float GetMaxHeight()       const { return maxHeight;       }
	float GetLandingDistance() const { return landingDistance; }
};