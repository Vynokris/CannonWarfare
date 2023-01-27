#include "CannonBall.h"
#include "PhysicsConstants.h"
#include "ParticleManager.h"
#include "Arithmetic.h"
#include "RaylibConversions.h"
#include <sstream>
#include <iomanip>
using namespace Maths;


CannonBall::CannonBall(ParticleManager& _particleManager, const Maths::Vector2& startPosition, const Maths::Vector2& startVelocity, const float& predictedAirTime, const float& _groundHeight)
	: particleManager(_particleManager), groundHeight(_groundHeight)
{
	transform.rotateForwards = true;
	transform.position = startPosition;
	transform.velocity = startVelocity;
	transform.acceleration = { 0, GRAVITY };
	startPos = transform.position;
	startV   = transform.velocity;

	startTime = std::chrono::system_clock::now();

	posHistory.emplace_back(ToRayVector2(transform.position));

	const SpawnerParticleParams params = {
		ParticleShapes::POLYGON,
		transform.position,
		-PI, 0,
		5, 20,
		0, 0,
		0, 0,
		20, 35,
		0.05f, 0.2f,
		ORANGE,
	};
	particleManager.CreateSpawner(1, predictedAirTime, params, &transform);
}

Maths::Vector2 CannonBall::ComputeDrag(const float& deltaTime) const
{
	const float dragCoeff = 0.5f * AIR_DENSITY * SPHERE_DRAG_COEFF * PI * sqpow(radius / PIXEL_SCALE);
	const float velocity  = transform.velocity.GetLength();
	return -transform.velocity * velocity * dragCoeff * deltaTime * 0.1f;
}

void CannonBall::SavePositionToHistory(const bool& forceSave)
{
	// Save the current position if it is far enough away from the previous one.
	if (!applyDrag) return;
	if (forceSave || posHistory.empty() || Maths::Vector2(FromRayVector2(posHistory.back()), transform.position).GetLengthSquared() > 2000.f)
		posHistory.emplace_back(ToRayVector2(transform.position));
}

void CannonBall::UpdateTrajectory()
{
	endTime = std::chrono::system_clock::now();
	airTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() / 1000.f;
	endPos  = transform.position;
	endV    = transform.velocity;
	controlPoint = LineIntersection(startPos, startV, endPos, -endV);
	SavePositionToHistory();
}

void CannonBall::UpdateColorAlphas(const float& deltaTime)
{
	if      ( showTrajectory && trajectoryAlpha < 1.f) trajectoryAlpha = clamp(trajectoryAlpha + deltaTime, 0, 1);
	else if (!showTrajectory && trajectoryAlpha > 0.f) trajectoryAlpha = clamp(trajectoryAlpha - deltaTime, 0, 1);
}

void CannonBall::UpdateDestroyTimer(const float& deltaTime)
{
	// Tick the timer down if it has started, and update transparency accordingly.
	if (0.f <= destroyTimer && destroyTimer <= destroyDuration)
	{
		destroyTimer -= deltaTime;
		color.a = (unsigned char)(255 * (destroyTimer / destroyDuration));
	}
}

void CannonBall::ApplyBouncingLogic()
{
	transform.acceleration = { 0, GRAVITY };
		
	// If it's the first ground it touches the ground, finalize the trajectory values.
	if (!landed)
	{
		transform.position.y = groundHeight - radius;
		SavePositionToHistory(true);
		UpdateTrajectory();
		landed = true;
	}

	// If it still has some velocity, make it bounce.
	if (transform.velocity.GetLength() > 10)
	{
		transform.position.y  = groundHeight - radius - 0.01f;
		transform.velocity.y *= -1;
		transform.velocity.SetLength(transform.velocity.GetLength() * elasticity);
	}

	// If it has very little velocity, stop all its movement.
	else
	{
		transform.position.y   = groundHeight - radius;
		transform.velocity     = {};
		transform.acceleration = {};
	}

	// Play landing particles.
	const float v = transform.velocity.GetLength();
	const SpawnerParticleParams params = {
		ParticleShapes::POLYGON,
		transform.position + Maths::Vector2(0, radius * 1.5f),
		-PI/4, PI+PI/2,
		v, 5 * v,
		0, 0,
		0, 0,
		20, 35,
		0.05f, 0.2f,
		WHITE,
	};
	particleManager.CreateSpawner(1, 0.1f, params);
}

void CannonBall::Update(const float& deltaTime)
{
	UpdateColorAlphas(deltaTime);
	UpdateDestroyTimer(deltaTime);
	
	// If the cannonball is above the ground, update its acceleration, velocity, position and trajectory.
	if (transform.position.y < groundHeight - radius)
	{
		if (applyDrag && !landed)
			transform.acceleration += ComputeDrag(deltaTime);
		transform.Update(deltaTime);

		if (!landed) UpdateTrajectory();
	}

	// If the cannonball is under the ground, make it bounce.
	else if (transform.position.y > groundHeight - radius)
	{
		ApplyBouncingLogic();
	}
}

void CannonBall::CheckCollisions(CannonBall* other)
{
	const Maths::Vector2 selfToOther = Maths::Vector2(transform.position, other->transform.position);
	const Maths::Vector2 dirToOther  = selfToOther.GetNormalized();
	const float          distToOther = selfToOther.GetLength();
	
	if (distToOther <= radius + other->radius)
	{
		// Get the masses of the projectiles.
		const float m1 = this ->mass;
		const float m2 = other->mass;

		// Get the initial velocities of the projectiles.
		const Maths::Vector2 v1i = this ->transform.velocity;
		const Maths::Vector2 v2i = other->transform.velocity;

		// Compute the final velocities of the projectiles.
		const Maths::Vector2 v1f = (v1i * (m1-m2) + v2i * (m2*2)) / (m1+m2);
		const Maths::Vector2 v2f = (v2i * (m2-m1) + v1i * (m1*2)) / (m1+m2);

		// Reset the projectiles' acceleration.
		this ->transform.acceleration = { 0, GRAVITY };
		other->transform.acceleration = { 0, GRAVITY };

		// Set the projectiles' velocities to the final velocities.
		this ->transform.velocity = v1f;
		other->transform.velocity = v2f;

		// Move the projectiles out of each other.
		this ->transform.position += dirToOther * (distToOther - (radius + other->radius)) / 2;
		other->transform.position -= dirToOther * (distToOther - (radius + other->radius)) / 2;

		// Tell the projectiles they have collided and should stop drawing their trajectory.
		if (!this ->landed) this ->collided = true;
		if (!other->landed) other->collided = true;

		// Play collision particles.
		const float v = (transform.velocity.GetLength() + other->transform.velocity.GetLength()) / 2;
		const SpawnerParticleParams params = {
			ParticleShapes::LINE,
			transform.position + dirToOther * radius,
			0, 2*PI,
			v, v*3,
			0, 0,
			0, 0,
			v/50, v/10,
			0.05f, 0.2f,
			color,
		};
		particleManager.CreateSpawner(5, 0.1f, params);
	}
}

void CannonBall::Draw() const
{
	// Draw the cannonball.
	DrawCircle     ((int)transform.position.x, (int)transform.position.y, radius, BLACK);
	DrawCircleLines((int)transform.position.x, (int)transform.position.y, radius, color);

	// Get the current trajectory color.
	const Color curColor = { color.r, color.g, color.b, (unsigned char)min(trajectoryAlpha * 255, color.a) };
	
	// Draw air time.
	std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << airTime << "s";
	const Maths::Vector2 textPos = { transform.position.x - MeasureText(textValue.str().c_str(), 20) / 2.f, transform.position.y - 10 };
	DrawText(textValue.str().c_str(), (int)textPos.x, (int)textPos.y, 20, curColor);
}

void CannonBall::DrawTrajectory()
{
	if (!collided)
	{
		const Color curColor = { color.r, color.g, color.b, (unsigned char)min(trajectoryAlpha * 255, color.a) };

		if (!applyDrag)
		{
			// Draw the trajectory with a bezier curve.
			DrawLineBezierQuad(ToRayVector2(startPos), ToRayVector2(endPos), ToRayVector2(controlPoint), 1, curColor);
		}
		else if (!posHistory.empty())
		{
			DrawLineStrip(posHistory.data(), (int)posHistory.size(), curColor);
			if (!landed)
				DrawLineV(posHistory.back(), ToRayVector2(transform.position), curColor);
		}

		// Draw the start circle and end arrow.
		DrawCircleV(ToRayVector2(startPos), 5, curColor);
		DrawPoly(ToRayVector2(endPos), 3, 12, radToDeg(endV.GetAngle()) - 90, curColor);
	}
}

void CannonBall::Destroy()
{
	// Start the destroy timer.
	destroyTimer = destroyDuration;
}
