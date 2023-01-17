#include "Cannon.h"

Cannon::Cannon()
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
	for (CannonBall* projectile : projectiles)
		projectile->Draw();
}

void Cannon::Shoot()
{
	projectiles.push_back(new CannonBall(position, Maths::Vector2(rotation, shootingVelocity, true)));
}
