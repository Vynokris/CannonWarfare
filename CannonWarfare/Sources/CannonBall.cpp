#include "CannonBall.h"
#include "PhysicsConstants.h"
#include "Arithmetic.h"
#include "RaylibConversions.h"
#include <sstream>
#include <iostream>
#include <iomanip>
using namespace Maths;


CannonBall::CannonBall(const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity, const float& _groundHeight)
	: position(startPosition), velocity(startVelocity), acceleration({ 0, GRAVITY }), groundHeight(_groundHeight)
{
	startPos = position;
	startV   = velocity;

	startTime = std::chrono::system_clock::now();
}

Maths::Vector2 CannonBall::ComputeDrag()
{
	float dragCoeff = 0.5f * AIR_DENSITY * SPHERE_DRAG_COEFF * PI * sqpow(radius);
	return (velocity * velocity.GetLength()) * -dragCoeff;
}

void CannonBall::Update(const float& deltaTime)
{
	if (position.y < groundHeight - 30)
	{
		velocity += acceleration * deltaTime;
		position += velocity * deltaTime;
	}
	else if (position.y > groundHeight - 30)
	{
		if (!landed)
		{
			landed  = true;
			endTime = std::chrono::system_clock::now();
			airTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.f;
			endPos  = position;
			endV    = velocity;
			controlPoint = LineIntersection(startPos, startV, endPos, -endV);
		}

		if (velocity.GetLength() <= 10)
		{
			position.y   = groundHeight - 30;
			velocity     = {};
			acceleration = {};
		}
		else
		{
			position.y  = groundHeight - 30.01f;
			velocity.y *= -1;
			velocity.SetLength(velocity.GetLength() * elasticity);
		}
	}

	if (0.f <= destroyCounter && destroyCounter <= destroyDuration)
	{
		destroyCounter -= deltaTime;
		color.a = (unsigned char)(255 * (destroyCounter / destroyDuration));
	}
}

void CannonBall::Draw()
{
	DrawCircleLines((int)position.x, (int)position.y, radius * 600, color);

	if (landed)
	{
		// Draw total trajectory.
		DrawLineBezierQuad(ToRayVector2(startPos), ToRayVector2(endPos), ToRayVector2(controlPoint), 1, color);

		// Draw total duration.
		std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << airTime << " seconds";
		const Maths::Vector2 textPos = { position.x - MeasureText(textValue.str().c_str(), 20) / 2.f, position.y - 60 };
		DrawText(textValue.str().c_str(), (int)textPos.x, (int)textPos.y, 20, color);
	}
	else
	{
		// Draw partial trajectory.
		controlPoint = LineIntersection(startPos, startV, position, -velocity);
		DrawLineBezierQuad(ToRayVector2(startPos), ToRayVector2(position), ToRayVector2(controlPoint), 1, color);
		
		// Draw current air time.
		endTime = std::chrono::system_clock::now();
		airTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.f;
		std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << airTime << " seconds";
		const Maths::Vector2 textPos = { position.x - MeasureText(textValue.str().c_str(), 20) / 2.f, position.y - 60 };
		DrawText(textValue.str().c_str(), (int)textPos.x, (int)textPos.y, 20, color);
	}
}

void CannonBall::Destroy()
{
	destroyCounter = destroyDuration;
}
