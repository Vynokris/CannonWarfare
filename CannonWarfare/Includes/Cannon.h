#pragma once
#include "CannonBall.h"
#include "raylib.h"
#include <vector>

class App;

struct CannonDrawPoints
{
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
	App* app;
	std::vector<CannonBall*> projectiles;
	const float& groundHeight;

	Maths::Vector2 position;
	float rotation = 0, shootingVelocity = 0;

	Maths::Vector2 landingPosition, controlPoint;
	float airTime = 0, maxHeight = 0, landingDistance = 0;

	Color trajectoryColor      = MAGENTA;
	Color landingDistanceColor = GREEN;
	Color maxHeightColor       = RED;

	CannonDrawPoints drawPoints;

	void UpdateTrajectory();
	void UpdateDrawPoints();

public:
	bool automaticRotation = true;

	Cannon(App* _app, const float& _groundHeight);
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