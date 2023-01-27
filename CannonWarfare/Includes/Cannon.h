#pragma once
#include "CannonBall.h"
#include "Physics/PhysicsConstants.h"
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

struct CannonProperties
{
	Maths::Vector2 anchorPos;
	float mass = 2500;                           // kg
	float barrelLength = 3.08f * PIXEL_SCALE;    // m -> px
	float powderCharge = 3.6f;                   // kg
	float chargeVelocity = 685.8f * PIXEL_SCALE; // m/s -> px/s
	float projectileRadius = 30;                 // px
	float projectileMass = 3.92f;                // kg
};

class Cannon
{
private:
	ParticleManager& particleManager;
	std::vector<CannonBall*> projectiles;
	const float& groundHeight;

	// Cannon properties.
	Maths::Transform2D transform;
	Maths::Vector2 shootingPoint;
	CannonProperties properties;

	// Predicted values for cannonballs.
	Maths::Vector2 landingVelocity, landingPosition, controlPoint, highestPoint;
	float airTime = 0, maxHeight = 0, landingDistance = 0;
	std::vector<::Vector2> posPredicted; // Used to draw trajectory with drag.
	
	CannonDrawParams drawParams;

public:
	bool automaticRotation = true;
	bool applyRecoil       = false;
	bool applyDrag         = false;
	bool applyCollisions   = false;
	bool showTrajectory    = true;
	bool showMeasurements  = true;
	bool showProjectileTrajectories = true;
	
private:
	void  UpdateDrawPoints();
	void  UpdateTrajectory();
	void  UpdateCollisions(CannonBall* cannonBall) const;
	void  ApplyRecoil();
	float ComputeMuzzleVelocity();

public:
	Cannon(ParticleManager& _particleManager, const float& _groundHeight);
	~Cannon();

	void Update(const float& deltaTime);
	void Draw() const;
	void DrawTrajectories();
	void DrawMeasurements() const;

	void Shoot();
	void ClearProjectiles() const;

	void SetAnchorPos(const Maths::Vector2&  pos) { properties.anchorPos          = pos;  UpdateTrajectory(); UpdateDrawPoints(); }
	void SetPosition (const Maths::Vector2&  pos) { transform.position            = pos;  UpdateTrajectory(); UpdateDrawPoints(); }
	void SetRotation (const float&           rot) { transform.rotation            = rot;  UpdateTrajectory(); UpdateDrawPoints(); }
	void SetProjectileRadius  (const float& rad ) { properties.projectileRadius   = rad;  UpdateTrajectory(); UpdateDrawPoints(); }
	void SetProjectileMass    (const float& mass) { properties.projectileMass     = mass; UpdateTrajectory(); }
	void SetBarrelLength      (const float& len ) { properties.barrelLength       = len;  UpdateTrajectory(); UpdateDrawPoints(); }
	void SetPowderCharge      (const float& mass) { properties.powderCharge       = mass; UpdateTrajectory(); }
	
	Maths::Vector2 GetAnchorPos()          const { return properties.anchorPos;          }
	Maths::Vector2 GetPosition()           const { return transform.position;            }
	float          GetRotation()           const { return transform.rotation;            }
	float          GetProjectileRadius()   const { return properties.projectileRadius;   }
	float          GetProjectileMass()     const { return properties.projectileMass;     }
	float          GetBarrelLength()       const { return properties.barrelLength;       }
	float          GetPowderCharge()       const { return properties.powderCharge;       }

	float GetAirTime()         const { return airTime;         }
	float GetMaxHeight()       const { return maxHeight;       }
	float GetLandingDistance() const { return landingDistance; }
};