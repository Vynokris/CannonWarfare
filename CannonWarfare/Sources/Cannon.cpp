#include "App.h"
#include "Cannon.h"
#include "PhysicsConstants.h"
#include "RaylibConversions.h"
#include <sstream>
#include <iomanip>
using namespace Maths;

Cannon::Cannon(ParticleManager& _particleManager, const float& _groundHeight)
       : particleManager(_particleManager), groundHeight(_groundHeight)
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
    const float c = shootingPoint.y - (groundHeight - 30);

    // Find the value for delta's square root.
    const float sqrtDelta = sqrt(sqpow(b) - 4*a*c);

    // Compute the equation's roots t1 and t2 and choose the highest one.
    const float t1 = (-b - sqrtDelta) / (2*a);
    const float t2 = (-b + sqrtDelta) / (2*a);
    const float t  = (t1 >= t2 ? t1 : t2);

    // Find the landing velocity using the cannonball's velocity equation: (v0.x, v0.y * g * t)
    landingVelocity = { v0.x, v0.y + GRAVITY * t };

    // Find the landing position using the cannonball's movement equation: (v0.x * t + p0.x, g * 0.5f * t^2 + v0.y * t + p0.y)
    landingPosition = { v0.x*t + shootingPoint.x, GRAVITY*0.5f*sqpow(t) + v0.y*t + shootingPoint.y };

    // Find the control point of the bezier curve linked to the cannonball's movement equation.
    // This is done by finding the intersection between the line following lines:
    // - line that passes through the start position in the direction of the start velocity
    // - line that passes through the landing point in the direction of the landing velocity
    controlPoint = LineIntersection(shootingPoint, v0, landingPosition, -landingVelocity);

    // Save the trajectory's general parameters.
    airTime = t;
    landingDistance = landingPosition.x - shootingPoint.x;
    const float highestPointT = clamp((shootingPoint.y - controlPoint.y) / (shootingPoint.y + landingPosition.y - 2*controlPoint.y), 0, 1);
    highestPoint = shootingPoint * sqpow(1-highestPointT) + controlPoint * 2*(1-highestPointT)*highestPointT + landingPosition * sqpow(highestPointT);
    maxHeight = clampAbove(shootingPoint.y - highestPoint.y, 0);
}

void Cannon::UpdateCollisions(CannonBall* cannonBall) const
{
    for (CannonBall* projectile : projectiles)
        if (cannonBall != projectile)
            cannonBall->CheckCollisions(projectile);
}

void Cannon::UpdateDrawPoints()
{
    drawParams.centerUp   = position               + Maths::Vector2(rotation-PI/2, 40, true);
    drawParams.centerDown = position               + Maths::Vector2(rotation+PI/2, 40, true);
    drawParams.midUp      = drawParams.centerUp   + Maths::Vector2(rotation+PI/20, 100, true);
    drawParams.midDown    = drawParams.centerDown + Maths::Vector2(rotation-PI/20, 100, true);
    drawParams.frontUp    = drawParams.midUp      + Maths::Vector2(rotation, 14, true);
    drawParams.frontDown  = drawParams.midDown    + Maths::Vector2(rotation, 14, true);
    
    drawParams.wickPointOffset        = Maths::Vector2(rotation, 40, true);
    drawParams.wickControlOffset = Maths::Vector2(rotation + PI/4, 20, true);
    drawParams.wick0 = position          - drawParams.wickPointOffset;
    drawParams.wick1 = drawParams.wick0 - drawParams.wickPointOffset;
    drawParams.wick2 = drawParams.wick1 + drawParams.wickControlOffset;
    drawParams.wick3 = drawParams.wick0 - drawParams.wickControlOffset;

    shootingPoint = (drawParams.frontUp + drawParams.frontDown) / 2.f;
}

void Cannon::Update(const float& deltaTime)
{
    // Automatically update cannon rotation.
    if (automaticRotation)
        SetRotation((sin(App::GetTimeSinceStart() * 0.25f) * 0.5f + 0.5f) * (-PI/3) - PI/8);

    // Update alphas depending on what is shown.
    if      ( showTrajectory   && drawParams.trajectoryAlpha   < 1.f) drawParams.trajectoryAlpha   = clamp(drawParams.trajectoryAlpha   + deltaTime, 0, 1);
    else if (!showTrajectory   && drawParams.trajectoryAlpha   > 0.f) drawParams.trajectoryAlpha   = clamp(drawParams.trajectoryAlpha   - deltaTime, 0, 1);
    if      ( showMeasurements && drawParams.measurementsAlpha < 1.f) drawParams.measurementsAlpha = clamp(drawParams.measurementsAlpha + deltaTime, 0, 1);
    else if (!showMeasurements && drawParams.measurementsAlpha > 0.f) drawParams.measurementsAlpha = clamp(drawParams.measurementsAlpha - deltaTime, 0, 1);

    // Update projectiles.
    for (size_t i = 0; i < projectiles.size(); i++) 
    {
        if (applyCollisions)
            UpdateCollisions(projectiles[i]);
        projectiles[i]->Update(deltaTime);

        // Set all projectiles to show/hide their trajectory.
        if (projectiles[i]->showTrajectory != showProjectileTrajectories)
            projectiles[i]->showTrajectory  = showProjectileTrajectories;

        // Tell all projectiles to apply/ignore drag.
        if (projectiles[i]->applyDrag != applyDrag)
            projectiles[i]->applyDrag  = applyDrag;

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
    DrawCircleSectorLines(ToRayVector2(position), 40, -degRot-90, -degRot+90, 10, drawParams.cannonColor);
    DrawLineEx           (ToRayVector2(drawParams.centerUp), ToRayVector2(drawParams.centerDown), 2,  BLACK);

    // Draw the barrel sides.
    DrawTriangle(ToRayVector2(drawParams.midDown   ), ToRayVector2(drawParams.midUp     ), ToRayVector2(drawParams.centerUp), BLACK);
    DrawTriangle(ToRayVector2(drawParams.centerUp  ), ToRayVector2(drawParams.centerDown), ToRayVector2(drawParams.midDown ), BLACK);
    DrawLineEx  (ToRayVector2(drawParams.centerUp  ), ToRayVector2(drawParams.midUp     ), 1, drawParams.cannonColor);
    DrawLineEx  (ToRayVector2(drawParams.centerDown), ToRayVector2(drawParams.midDown   ), 1, drawParams.cannonColor);

    // Draw the sides of the tip of the barrel.
    DrawCircleSector     (ToRayVector2((drawParams.midUp   + drawParams.frontUp  ) / 2), 7, -degRot-180, -degRot, 10, BLACK);
    DrawCircleSector     (ToRayVector2((drawParams.midDown + drawParams.frontDown) / 2), 7, -degRot, -degRot+180, 10, BLACK);
    DrawCircleSectorLines(ToRayVector2((drawParams.midUp   + drawParams.frontUp  ) / 2), 7, -degRot-180, -degRot, 10, drawParams.cannonColor);
    DrawCircleSectorLines(ToRayVector2((drawParams.midDown + drawParams.frontDown) / 2), 7, -degRot, -degRot+180, 10, drawParams.cannonColor);
    DrawLineEx(ToRayVector2(drawParams.midUp  ), ToRayVector2(drawParams.frontUp  ), 2, BLACK);
    DrawLineEx(ToRayVector2(drawParams.midDown), ToRayVector2(drawParams.frontDown), 2, BLACK);

    // Draw the tip of the barrel.
    DrawTriangle(ToRayVector2(drawParams.frontDown), ToRayVector2(drawParams.frontUp  ), ToRayVector2(drawParams.midUp    ), BLACK);
    DrawTriangle(ToRayVector2(drawParams.midUp    ), ToRayVector2(drawParams.midDown  ), ToRayVector2(drawParams.frontDown), BLACK);
    DrawLineEx  (ToRayVector2(drawParams.midUp    ), ToRayVector2(drawParams.midDown  ), 1, drawParams.cannonColor);
    DrawLineEx  (ToRayVector2(drawParams.frontUp  ), ToRayVector2(drawParams.frontDown), 1, drawParams.cannonColor);

    // Draw the wick.
    DrawLineBezierCubic(ToRayVector2(drawParams.wick0),
                        ToRayVector2(drawParams.wick1),
                        ToRayVector2(drawParams.wick2),
                        ToRayVector2(drawParams.wick3), 1, drawParams.cannonColor);
}

void Cannon::DrawTrajectories() const
{
    // Draw the trajectory.
    const Color curColor = { drawParams.trajectoryColor.r,
                             drawParams.trajectoryColor.g,
                             drawParams.trajectoryColor.b,
                             (unsigned char)(drawParams.trajectoryAlpha * 255) };
    DrawLineBezierQuad(ToRayVector2(shootingPoint), ToRayVector2(landingPosition), ToRayVector2(controlPoint), 1, curColor);
    DrawPoly(ToRayVector2(landingPosition), 3, 12, radToDeg(landingVelocity.GetAngle()) - 90, curColor);
    
    // Draw the cannonball trajectories.
    for (const CannonBall* projectile : projectiles)
        projectile->DrawTrajectory();
}

void Cannon::DrawMeasurements() const
{
    // Draw the air time text.
    {
        const Color curColor = { drawParams.trajectoryColor.r,
                                 drawParams.trajectoryColor.g,
                                 drawParams.trajectoryColor.b,
                                 (unsigned char)(drawParams.trajectoryAlpha * 255) };
        std::stringstream textValue; textValue << std::fixed << std::setprecision(2) << airTime << "s";
        const Maths::Vector2 textPos = { highestPoint.x - MeasureText(textValue.str().c_str(), 30) / 2.f, highestPoint.y - 35 };
        DrawText(textValue.str().c_str(), (int)textPos.x, (int)textPos.y, 30, curColor);
    }
    
    // Draw the landing distance.
    {
        const Color curColor = { drawParams.landingDistanceColor.r,
                                 drawParams.landingDistanceColor.g,
                                 drawParams.landingDistanceColor.b,
                                 (unsigned char)(drawParams.measurementsAlpha * 255) };
        DrawLine((int)shootingPoint.x, (int)groundHeight + 20, (int)(shootingPoint.x + landingDistance), (int)groundHeight + 20, curColor);
        DrawPoly({ shootingPoint.x                   + 12, groundHeight + 20 }, 3, 12,  90, curColor);
        DrawPoly({ shootingPoint.x + landingDistance - 12, groundHeight + 20 }, 3, 12, -90, curColor);
        std::stringstream textValue; textValue << std::fixed << std::setprecision(0) << landingDistance << "px";
        DrawText(textValue.str().c_str(), (int)(shootingPoint.x + landingDistance / 2 - MeasureText(textValue.str().c_str(), 30) / 2.f), (int)groundHeight + 30, 30, curColor);
    }

    // Draw the maximum height.
    {
        const Color curColor = { drawParams.maxHeightColor.r,
                                 drawParams.maxHeightColor.g,
                                 drawParams.maxHeightColor.b,
                                 (unsigned char)(drawParams.measurementsAlpha * 255) };
        DrawLine(30, (int)shootingPoint.y, 30, (int)(shootingPoint.y - maxHeight), curColor);
        DrawPoly({ 30, shootingPoint.y             - 12 }, 3, 12,   0, curColor);
        DrawPoly({ 30, shootingPoint.y - maxHeight + 12 }, 3, 12, 180, curColor);
        std::stringstream textValue; textValue << std::fixed << std::setprecision(0) << maxHeight << "px";
        DrawText(textValue.str().c_str(), 15, (int)(shootingPoint.y - maxHeight) - 30, 30, curColor);
    }
}

void Cannon::Shoot()
{
    const SpawnerParticleParams params = {
        ParticleShapes::LINE,
        (drawParams.frontUp + drawParams.frontDown) / 2,
        rotation - PI/2, rotation + PI/2,
        shootingVelocity / 4, shootingVelocity,
        0, 0,
        0, 0,
        20, 50,
        0.05f, 0.2f,
        ORANGE,
    };
    particleManager.CreateSpawner(20, 0.2f, params);
    
    projectiles.push_back(new CannonBall(particleManager, shootingPoint, Maths::Vector2(rotation, shootingVelocity, true), airTime, groundHeight));
    if (projectiles.size() > MAX_PROJECTILES)
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
