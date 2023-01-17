#include "Cannon.h"
#include "RaylibConversions.h"
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

void Cannon::Update(const float& deltaTime)
{
	for (CannonBall* projectile : projectiles)
		projectile->Update(deltaTime);
}

void Cannon::Draw()
{
	DrawPolyLines(ToRayVector2(position), 4, 50, radToDeg(rotation) - 45, WHITE);

	for (CannonBall* projectile : projectiles)
		projectile->Draw();
}

void Cannon::Shoot()
{
	projectiles.push_back(new CannonBall(position, Maths::Vector2(rotation, shootingVelocity, true), groundHeight));
}
