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
	for (const CannonBall* projectile : projectiles)
		delete projectile;
	projectiles.clear();
}

void Cannon::UpdateTrajectory()
{
	// Find the time (t) at which a cannonball would hit the ground.
	// This is done by finding when the cannonball's vertical movement equation (a.y*0.5*t^2 + v0.y*t + p0.y) is equal to groundHeight
	// It's the same as solving the following equation by finding its roots: a.y*0.5*t^2 + v0.y*t + p0.y - groundHeight
	const Maths::Vector2 v0 = Maths::Vector2(rotation, shootingVelocity, true);

	// The three coefficients of the equation: a*t^2 + b*t + c
	const float a = GRAVITY * 0.5f;
	const float b = v0.y;
	const float c = position.y - groundHeight;

	// Find the value for delta's square root.
	const float sqrtDelta = sqrt(sqpow(b) - 4*a*c);

	// Compute the equation's roots t1 and t2 and choose the highest one.
	const float t1 = (-b - sqrtDelta) / (2*a);
	const float t2 = (-b + sqrtDelta) / (2*a);
	const float t  = (t1 >= t2 ? t1 : t2);

	// Find the landing velocity using the cannonball's velocity equation: (v0.x, v0.y * g * t)
	const Maths::Vector2 landingVelocity = { v0.x, v0.y + GRAVITY * t };

	// Find the landing position using the cannonball's movement equation: (v0.x * t + p0.x, g * 0.5f * t^2 + v0.y * t + p0.y)
	landingPosition = { v0.x*t + position.x, GRAVITY*0.5f*sqpow(t) + v0.y*t + position.y };

	// Find the control point of the bezier curve linked to the cannonball's movement equation.
	// This is done by finding the intersection between the line following lines:
	// - line that passes through the start position in the direction of the start velocity
	// - line that passes through the landing point in the direction of the landing velocity
	controlPoint = LineIntersection(position, v0, landingPosition, -landingVelocity);

	// Save the trajectory's general parameters.
	airTime = t;
	landingDistance = landingPosition.x - position.x;
	maxHeight = clampAbove(position.y - (position * sqpow(1-0.5f) + controlPoint * 2*(1-0.5f)*0.5f + landingPosition * sqpow(0.5f)).y, 0); // TODO: This is broken with non-0 height.
}

void Cannon::UpdateDrawPoints()
{
	drawPoints.centerUp   = position               + Maths::Vector2(rotation-PI/2, 40, true);
	drawPoints.centerDown = position               + Maths::Vector2(rotation+PI/2, 40, true);
	drawPoints.midUp      = drawPoints.centerUp   + Maths::Vector2(rotation+PI/20, 100, true);
	drawPoints.midDown    = drawPoints.centerDown + Maths::Vector2(rotation-PI/20, 100, true);
	drawPoints.frontUp    = drawPoints.midUp      + Maths::Vector2(rotation, 14, true);
	drawPoints.frontDown  = drawPoints.midDown    + Maths::Vector2(rotation, 14, true);
	
	drawPoints.wickPointOffset        = Maths::Vector2(rotation, 40, true);
	drawPoints.wickControlOffset = Maths::Vector2(rotation + PI/4, 20, true);
	drawPoints.wick0 = position          - drawPoints.wickPointOffset;
	drawPoints.wick1 = drawPoints.wick0 - drawPoints.wickPointOffset;
	drawPoints.wick2 = drawPoints.wick1 + drawPoints.wickControlOffset;
	drawPoints.wick3 = drawPoints.wick0 - drawPoints.wickControlOffset;
}

void Cannon::Update(const float& deltaTime)
{
	// Automatically update cannon rotation.
	if (automaticRotation)
		SetRotation((sin(App::GetTimeSinceStart() * 0.25f) * 0.5f + 0.5f) * (-PI/3) - PI/8);

	// Update alphas depending on what is shown.
	if      ( showTrajectory   && trajectoryAlpha   < 1.f) trajectoryAlpha   = clamp(trajectoryAlpha   + deltaTime, 0, 1);
	else if (!showTrajectory   && trajectoryAlpha   > 0.f) trajectoryAlpha   = clamp(trajectoryAlpha   - deltaTime, 0, 1);
	if      ( showMeasurements && measurementsAlpha < 1.f) measurementsAlpha = clamp(measurementsAlpha + deltaTime, 0, 1);
	else if (!showMeasurements && measurementsAlpha > 0.f) measurementsAlpha = clamp(measurementsAlpha - deltaTime, 0, 1);

	// Update projectiles.
	for (size_t i = 0; i < projectiles.size(); i++) 
	{
		projectiles[i]->Update(deltaTime);

		if (projectiles[i]->showTrajectory != showProjectileTrajectories)
			projectiles[i]->showTrajectory  = showProjectileTrajectories;

		// Delete any projectile that has finished destroying itself.
		if (projectiles[i]->IsDestroyed()) {
			delete projectiles[i];
			projectiles.erase(projectiles.begin() + i);
			i -= 1;
		}
	}
}

void Cannon::Draw() const
{
	// Draw the cannonballs.
	for (const CannonBall* projectile : projectiles)
		projectile->Draw();
	
	// Draw the back semi-circle.
	const float degRot = radToDeg(rotation) + 90;
	DrawCircleSector     (ToRayVector2(position), 40, -degRot-90, -degRot+90, 10, BLACK);
	DrawCircleSectorLines(ToRayVector2(position), 40, -degRot-90, -degRot+90, 10, WHITE);
	DrawLineEx           (ToRayVector2(drawPoints.centerUp), ToRayVector2(drawPoints.centerDown), 2,  BLACK);

	// Draw the barrel sides.
	DrawTriangle(ToRayVector2(drawPoints.midDown   ), ToRayVector2(drawPoints.midUp     ), ToRayVector2(drawPoints.centerUp), BLACK);
	DrawTriangle(ToRayVector2(drawPoints.centerUp  ), ToRayVector2(drawPoints.centerDown), ToRayVector2(drawPoints.midDown ), BLACK);
	DrawLineEx  (ToRayVector2(drawPoints.centerUp  ), ToRayVector2(drawPoints.midUp     ), 1, WHITE);
	DrawLineEx  (ToRayVector2(drawPoints.centerDown), ToRayVector2(drawPoints.midDown   ), 1, WHITE);

	// Draw the sides of the tip of the barrel.
	DrawCircleSector     (ToRayVector2((drawPoints.midUp   + drawPoints.frontUp  ) / 2), 7, -degRot-180, -degRot, 10, BLACK);
	DrawCircleSector     (ToRayVector2((drawPoints.midDown + drawPoints.frontDown) / 2), 7, -degRot, -degRot+180, 10, BLACK);
	DrawCircleSectorLines(ToRayVector2((drawPoints.midUp   + drawPoints.frontUp  ) / 2), 7, -degRot-180, -degRot, 10, WHITE);
	DrawCircleSectorLines(ToRayVector2((drawPoints.midDown + drawPoints.frontDown) / 2), 7, -degRot, -degRot+180, 10, WHITE);
	DrawLineEx(ToRayVector2(drawPoints.midUp  ), ToRayVector2(drawPoints.frontUp  ), 2, BLACK);
	DrawLineEx(ToRayVector2(drawPoints.midDown), ToRayVector2(drawPoints.frontDown), 2, BLACK);

	// Draw the tip of the barrel.
	DrawTriangle(ToRayVector2(drawPoints.frontDown), ToRayVector2(drawPoints.frontUp  ), ToRayVector2(drawPoints.midUp    ), BLACK);
	DrawTriangle(ToRayVector2(drawPoints.midUp    ), ToRayVector2(drawPoints.midDown  ), ToRayVector2(drawPoints.frontDown), BLACK);
	DrawLineEx  (ToRayVector2(drawPoints.midUp    ), ToRayVector2(drawPoints.midDown  ), 1, WHITE);
	DrawLineEx  (ToRayVector2(drawPoints.frontUp  ), ToRayVector2(drawPoints.frontDown), 1, WHITE);

	// Draw the wick.
	DrawLineBezierCubic(ToRayVector2(drawPoints.wick0),
						ToRayVector2(drawPoints.wick1),
						ToRayVector2(drawPoints.wick2),
						ToRayVector2(drawPoints.wick3), 1, WHITE);
}

void Cannon::DrawTrajectories() const
{
	// Draw the trajectory.
	const Color curTrajectoryColor = { trajectoryColor.r, trajectoryColor.g, trajectoryColor.b, (unsigned char)(trajectoryAlpha * 255) };
	DrawLineBezierQuad(ToRayVector2(position), ToRayVector2(landingPosition), ToRayVector2(controlPoint), 1, curTrajectoryColor);

	// Draw the air time text.
	{
		std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << airTime << "s";
		const Maths::Vector2 textPos = { controlPoint.x - MeasureText(textValue.str().c_str(), 30) / 2.f, position.y - maxHeight - 35 };
		DrawText(textValue.str().c_str(), (int)textPos.x, (int)textPos.y, 30, curTrajectoryColor);
	}
	
	// Draw the cannonball trajectories.
	for (const CannonBall* projectile : projectiles)
		projectile->DrawTrajectory();
}

void Cannon::DrawMeasurements() const
{
	// Draw the landing distance.
	{
		const Color curColor = { landingDistanceColor.r, landingDistanceColor.g, landingDistanceColor.b, (unsigned char)(measurementsAlpha * 255) };
		DrawLine((int)position.x, (int)groundHeight + 20, (int)(position.x + landingDistance), (int)groundHeight + 20, curColor);
		DrawPoly({ position.x                   + 12, groundHeight + 20 }, 3, 12,  90, curColor);
		DrawPoly({ position.x + landingDistance - 12, groundHeight + 20 }, 3, 12, -90, curColor);
		std::stringstream textValue; textValue << std::fixed << std::setprecision(0) << landingDistance << "px";
		DrawText(textValue.str().c_str(), (int)(position.x + landingDistance / 2 - MeasureText(textValue.str().c_str(), 30) / 2.f), (int)groundHeight + 30, 30, curColor);
	}

	// Draw the maximum height.
	{
		const Color curColor = { maxHeightColor.r, maxHeightColor.g, maxHeightColor.b, (unsigned char)(measurementsAlpha * 255) };
		DrawLine(30, (int)position.y, 30, (int)(position.y - maxHeight), curColor);
		DrawPoly({ 30, position.y             - 12 }, 3, 12,   0, curColor);
		DrawPoly({ 30, position.y - maxHeight + 12 }, 3, 12, 180, curColor);
		std::stringstream textValue; textValue << std::fixed << std::setprecision(0) << maxHeight << "px";
		DrawText(textValue.str().c_str(), 15, (int)(position.y - maxHeight) - 30, 30, curColor);
	}
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

void Cannon::ClearProjectiles() const
{
	for (CannonBall* projectile : projectiles)
	{
		if (!projectile->IsDestroying())
			projectile->Destroy();
	}
}
