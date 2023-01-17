#include "CannonBall.h"
#include "PhysicsConstants.h"
#include "Arithmetic.h"
#include "RaylibConversions.h"
#include <sstream>
#include <iomanip>
using namespace Maths;


CannonBall::CannonBall(const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity, const float& _groundHeight)
	: position(startPosition), velocity(startVelocity), acceleration({ 0, GRAVITY }), groundHeight(_groundHeight)
{
	startPos = position;
	startV   = velocity;

	startTime = std::chrono::system_clock::now();
}

Maths::Vector2 CannonBall::ComputeDrag() const
{
	const float dragCoeff = 0.5f * AIR_DENSITY * SPHERE_DRAG_COEFF * PI * sqpow(radius);
	return (velocity * velocity.GetLength()) * -dragCoeff;
}

void CannonBall::UpdateTrajectory()
{
	endTime = std::chrono::system_clock::now();
	airTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.f;
	endPos  = position;
	endV    = velocity;
	controlPoint = LineIntersection(startPos, startV, endPos, -endV);
}

void CannonBall::Update(const float& deltaTime)
{
	// If the cannonball is above the ground, update its velocity and position.
	if (position.y < groundHeight - radius * PIXEL_SCALE)
	{
		velocity += acceleration * deltaTime;
		position += velocity * deltaTime;

		if (!landed) UpdateTrajectory();
	}

	// If the cannonball is under the ground...
	else if (position.y > groundHeight - radius * PIXEL_SCALE)
	{
		// If it's the first ground it touches the ground, finalize the trajectory values.
		if (!landed)
		{
			UpdateTrajectory();
			landed = true;
		}

		// If it still has some velocity, make it bounce.
		if (velocity.GetLength() > 10)
		{
			position.y  = groundHeight - radius * PIXEL_SCALE - 0.01f;
			velocity.y *= -1;
			velocity.SetLength(velocity.GetLength() * elasticity);
		}

		// If it has very little velocity, stop all its movement.
		else
		{
			position.y   = groundHeight - radius * PIXEL_SCALE;
			velocity     = {};
			acceleration = {};
		}
	}

	// Update the destroy timer if it has started.
	if (0.f <= destroyTimer && destroyTimer <= destroyDuration)
	{
		destroyTimer -= deltaTime;
		color.a = (unsigned char)(255 * (destroyTimer / destroyDuration));
	}
}

void CannonBall::Draw() const
{
	// Draw the cannonball.
	DrawCircle((int)position.x, (int)position.y, radius * PIXEL_SCALE, BLACK);
	DrawCircleLines((int)position.x, (int)position.y, radius * PIXEL_SCALE, color);
}

void CannonBall::DrawTrajectory() const
{
	// Draw the trajectory with a bezier curve.
	DrawLineBezierQuad(ToRayVector2(startPos), ToRayVector2(endPos), ToRayVector2(controlPoint), 1, color);
}

void CannonBall::DrawAirTime() const
{
	// Draw air time.
	std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << airTime << "s";
	const Maths::Vector2 textPos = { position.x - MeasureText(textValue.str().c_str(), 20) / 2.f, position.y - 10 };
	DrawText(textValue.str().c_str(), (int)textPos.x, (int)textPos.y, 20, color);
}

void CannonBall::Destroy()
{
	// Start the destroy timer.
	destroyTimer = destroyDuration;
}
