#include "App.h"
#include "Cannon.h"
#include "PhysicsConstants.h"
#include "RaylibConversions.h"
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace Maths;

Cannon::Cannon(const float& _groundHeight)
	   : groundHeight(_groundHeight)
{
}

Cannon::~Cannon()
{
	for (CannonBall* projectile : projectiles)
		delete projectile;
	projectiles.clear();
}

void Cannon::UpdateTrajectory()
{
	// Find the time (t) at which a cannonball would hit the ground.
	// This is done by finding when the cannonball's vertical movement equation (a.y*0.5*t^2 + v0.y*t + p0.y) is equal to groundHeight
	// It's the same as solving the following equation by finding its roots: a.y*0.5*t^2 + v0.y*t + p0.y - groundHeight

	Maths::Vector2 v0 = Maths::Vector2(rotation, shootingVelocity, true);

	// The three coefficients of the equation: a*t^2 + b*t + c
	float a = GRAVITY * 0.5f;
	float b = v0.y;
	float c = position.y - groundHeight;

	// Find the value for delta's square root.
	float sqrtDelta = sqrt(sqpow(b) - 4*a*c);

	// Compute the equation's roots t1 and t2 and choose the highest one.
	float t1 = (-b - sqrtDelta) / (2*a);
	float t2 = (-b + sqrtDelta) / (2*a);
	float t  = (t1 >= t2 ? t1 : t2);

	// Find the landing velocity using the cannonball's velocity equation: (v0.x, v0.y * g * t)
	Maths::Vector2 landVel = { v0.x, v0.y + GRAVITY * t };

	// Find the landing position using the cannonball's movement equation: (v0.x * t + p0.x, g * 0.5f * t^2 + v0.y * t + p0.y)
	landingPosition = { v0.x*t + position.x, GRAVITY*0.5f*sqpow(t) + v0.y*t + position.y };

	// Find the control point of the bezier curve linked to the cannonball's movement equation.
	// This is done by finding the intersection between the line following lines:
	// - line that passes through the start position in the direction of the start velocity
	// - line that passes through the landing point in the direction of the landing velocity
	controlPoint = LineIntersection(position, v0, landingPosition, -landVel);

	// Save the trajectory's general parameters.
	airTime = t;
	landingDistance = landingPosition.x - position.x;
	maxHeight = clampAbove(position.y - (position * sqpow(1-0.5f) + controlPoint * 2*(1-0.5f)*0.5f + landingPosition * sqpow(0.5f)).y, 0);
}

void Cannon::Update(const float& deltaTime)
{
	if (automaticRotation)
	{
		SetRotation((sin(App::GetTimeSinceStart() * 0.25f) * 0.5f + 0.5f) * (-PI/3) - PI/8);
	}

	for (size_t i = 0; i < projectiles.size(); i++) 
	{
		projectiles[i]->Update(deltaTime);
		if (projectiles[i]->IsDestroyed()) {
			projectiles.erase(projectiles.begin() + i);
			i -= 1;
		}
	}
}

void Cannon::Draw()
{
	// Draw the landing distance.
	{
		DrawLine((int)position.x, (int)groundHeight + 20, (int)(position.x + landingDistance), (int)groundHeight + 20, landingDistanceColor);
		DrawPoly({ position.x                   + 12, groundHeight + 20 }, 3, 12,  90, landingDistanceColor);
		DrawPoly({ position.x + landingDistance - 12, groundHeight + 20 }, 3, 12, -90, landingDistanceColor);
		std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << landingDistance << " pixels";
		DrawText(textValue.str().c_str(), (int)(position.x + landingDistance / 2 - MeasureText(textValue.str().c_str(), 20) / 2.f), (int)groundHeight + 30, 20, landingDistanceColor);
	}

	// Draw the maximum height.
	{
		DrawLine(30, (int)position.y, 30, (int)(position.y - maxHeight), maxHeightColor);
		DrawPoly({ 30, position.y             - 12 }, 3, 12,   0, maxHeightColor);
		DrawPoly({ 30, position.y - maxHeight + 12 }, 3, 12, 180, maxHeightColor);
		std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << maxHeight << " pixels";
		DrawText(textValue.str().c_str(), 15, (int)(position.y - maxHeight) - 25, 20, maxHeightColor);
	}

	// Draw the air time text.
	{
		std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << airTime << " seconds";
		const Maths::Vector2 textPos = { controlPoint.x - MeasureText(textValue.str().c_str(), 20) / 2.f, position.y - maxHeight - 30 };
		DrawText(textValue.str().c_str(), (int)textPos.x, (int)textPos.y, 20, trajectoryColor);
	}

	// Draw the trajectory.
	DrawLineBezierQuad(ToRayVector2(position), ToRayVector2(landingPosition), ToRayVector2(controlPoint), 1, trajectoryColor);

	// Draw the cannon.
	DrawPolyLines(ToRayVector2(position), 4, 50, radToDeg(rotation) - 45, WHITE);

	// Draw the cannonballs.
	for (CannonBall* projectile : projectiles)
		projectile->Draw();
}

void Cannon::Shoot()
{
	projectiles.push_back(new CannonBall(position, Maths::Vector2(rotation, shootingVelocity, true), groundHeight));
	if (projectiles.size() > 4)
	{
		for (size_t i = 0; i < projectiles.size(); i++)
		{
			if (!projectiles[i]->IsDestroying())
			{
				projectiles[i]->Destroy();
				break;
			}
		}
	}
}

void Cannon::ClearProjectiles()
{
	for (size_t i = 0; i < projectiles.size(); i++)
	{
		projectiles[i]->Destroy();
	}
}
