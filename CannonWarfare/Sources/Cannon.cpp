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

void Cannon::UpdateDrawPoints()
{
    const float barrelRadius = properties.projectileRadius + 20;
    const float barrelLength = properties.barrelLength / PIXEL_SCALE * 50;
    const float barrelAngle  = atan((barrelRadius - properties.projectileRadius) / barrelLength);

    drawParams.centerUp   = transform.position    + Maths::Vector2(transform.rotation-PI/2, barrelRadius, true);
    drawParams.centerDown = transform.position    + Maths::Vector2(transform.rotation+PI/2, barrelRadius, true);
    drawParams.midUp      = drawParams.centerUp   + Maths::Vector2(transform.rotation+barrelAngle, barrelLength, true);
    drawParams.midDown    = drawParams.centerDown + Maths::Vector2(transform.rotation-barrelAngle, barrelLength, true);
    drawParams.frontUp    = drawParams.midUp      + Maths::Vector2(transform.rotation, 14, true);
    drawParams.frontDown  = drawParams.midDown    + Maths::Vector2(transform.rotation, 14, true);
    
    drawParams.wickPointOffset   = Maths::Vector2(transform.rotation, barrelRadius, true);
    drawParams.wickControlOffset = Maths::Vector2(transform.rotation + PI/4, (barrelRadius) * 0.5f, true);
    drawParams.wick0 = transform.position - drawParams.wickPointOffset;
    drawParams.wick1 = drawParams.wick0   - drawParams.wickPointOffset;
    drawParams.wick2 = drawParams.wick1   + drawParams.wickControlOffset;
    drawParams.wick3 = drawParams.wick0   - drawParams.wickControlOffset;

    shootingPoint = (drawParams.frontUp + drawParams.frontDown) / 2.f;
}

void Cannon::UpdateTrajectory()
{
    if (!applyDrag)
    {
        // Find the time (t) at which a cannonball would hit the ground.
        // This is done by finding when the cannonball's vertical movement equation (a.y*0.5*t^2 + v0.y*t + p0.y) is equal to groundHeight
        // It's the same as solving the following equation by finding its roots: a.y*0.5*t^2 + v0.y*t + p0.y - groundHeight
        const Maths::Vector2 v0 = Maths::Vector2(transform.rotation, ComputeMuzzleVelocity(), true);

        // The three coefficients of the equation: a*t^2 + b*t + c
        const float a = GRAVITY * 0.5f;
        const float b = v0.y;
        const float c = shootingPoint.y - (groundHeight - properties.projectileRadius);

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
    else
    {
        const float timeStep = 0.01f; airTime = 0; highestPoint.y = groundHeight;
        Transform2D projectileTransform = { shootingPoint, { transform.rotation, ComputeMuzzleVelocity(), true }, { 0, GRAVITY }, 0, 0, true };
        posPredicted.clear(); posPredicted.emplace_back(ToRayVector2(projectileTransform.position));

        // Simulate a projectile with drag until it hits the ground.
        while (projectileTransform.position.y < groundHeight - properties.projectileRadius)
        {
            // Increment air time.
            airTime += timeStep;

            // Apply drag.
            const float dragCoeff = 0.5f * AIR_DENSITY * SPHERE_DRAG_COEFF * PI * sqpow(properties.projectileRadius / PIXEL_SCALE);
            const float velocity  = projectileTransform.velocity.GetLength();
            projectileTransform.acceleration -= projectileTransform.velocity * velocity * dragCoeff * timeStep * 0.1f;

            // Update apply acceleration to velocity and velocity to acceleration.
            projectileTransform.Update(timeStep);

            // Save cannonball positions.
            if (Maths::Vector2(FromRayVector2(posPredicted.back()), projectileTransform.position).GetLengthSquared() > 500.f)
                posPredicted.emplace_back(ToRayVector2(projectileTransform.position));

            // Stop simulation once the cannonball hits the ground.
            if (projectileTransform.position.y < highestPoint.y)
                highestPoint = projectileTransform.position;
        }

        // Save its final velocity, position, and maximum height.
        landingVelocity = projectileTransform.velocity;
        landingPosition = { projectileTransform.position.x, groundHeight - properties.projectileRadius };
        landingDistance = landingPosition.x - shootingPoint.x;
        maxHeight = clampAbove(shootingPoint.y - highestPoint.y, 0);
        posPredicted.emplace_back(ToRayVector2(landingPosition));
    }
}

void Cannon::UpdateCollisions(CannonBall* cannonBall) const
{
    for (CannonBall* projectile : projectiles)
        if (cannonBall != projectile)
            cannonBall->CheckCollisions(projectile);
}

void Cannon::ApplyRecoil()
{
    if (applyRecoil)
    {
        // See this link for more info: https://www.omnicalculator.com/physics/recoil-energy
        float velocity = (sqpow(properties.projectileMass) * ComputeMuzzleVelocity() + properties.powderCharge * properties.chargeVelocity) / properties.mass;
        transform.velocity = -Maths::Vector2(transform.rotation, velocity, true);
    }
}

float Cannon::ComputeMuzzleVelocity()
{
    // See this link for more info: https://www.arc.id.au/CannonBallistics.html
    const float d = properties.projectileRadius * 2; // Barrel diameter (px)
    const float m = properties.projectileMass; // (kg)
    const float p = properties.powderCharge; // (kg)
    const float L = properties.barrelLength; // (px)
    const float eta = 881    / PIXEL_SCALE;  // Density of powder (kg/m^3 -> kg/px^3)
    const float rho = 997    / PIXEL_SCALE;  // Density of water (kg/m^3 -> kg/px^3)
    const float atm = 1.225f / PIXEL_SCALE;  // Atmospheric pressure (kg/m^3 -> kg/px^3)
    const float R   = 1600;                  // Gunpowder gas pressure to atm ratio
    const float c   = p*4/(PI*sqpow(d)*eta); // Length of charge

    const float v = sqrt(2*R*atm/eta) * sqrt(p/(m+p/3) * log(L/c));
    return v * 130;
}

void Cannon::Update(const float& deltaTime)
{
    if (applyRecoil)
    {
        transform.Update(deltaTime);
        transform.position.y = clampUnder(transform.position.y, groundHeight);
        const Maths::Vector2 posToAnchor = Maths::Vector2(transform.position, properties.anchorPos);
        transform.velocity -= transform.velocity * deltaTime * 10;
        if (transform.velocity.GetLengthSquared() > 0.1f && posToAnchor.GetLengthSquared() > 0.01f) {
            transform.position += posToAnchor * deltaTime * 10 * (1 / transform.velocity.GetLengthSquared());
            UpdateDrawPoints();
            UpdateTrajectory();
        }
    }

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
    const float degRot       = radToDeg(transform.rotation) + 90;
    const float circleRadius = Maths::Vector2(transform.position, drawParams.centerUp).GetLength();
    DrawCircleSector     (ToRayVector2(transform.position), circleRadius, -degRot-90, -degRot+90, 10, BLACK);
    DrawCircleSectorLines(ToRayVector2(transform.position), circleRadius, -degRot-90, -degRot+90, 10, drawParams.cannonColor);
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

void Cannon::DrawTrajectories()
{
    const Color curColor = { drawParams.trajectoryColor.r,
                             drawParams.trajectoryColor.g,
                             drawParams.trajectoryColor.b,
                             (unsigned char)(drawParams.trajectoryAlpha * 255) };
    
    // Draw the trajectory.
    if (!applyDrag)
        DrawLineBezierQuad(ToRayVector2(shootingPoint), ToRayVector2(landingPosition), ToRayVector2(controlPoint), 1, curColor);
    else
        DrawLineStrip(posPredicted.data(), (int)posPredicted.size(), curColor);

    // Draw the arrow at the end of the trajectory.
    DrawPoly(ToRayVector2(landingPosition), 3, 12, radToDeg(landingVelocity.GetAngle()) - 90, curColor);
    
    // Draw the cannonball trajectories.
    for (CannonBall* projectile : projectiles)
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
    const float projectileVelocity = ComputeMuzzleVelocity();

    // Play shooting particles.
    const SpawnerParticleParams params = {
        ParticleShapes::LINE,
        (drawParams.frontUp + drawParams.frontDown) / 2,
        transform.rotation - PI/2, transform.rotation + PI/2,
        projectileVelocity / 4, projectileVelocity,
        0, 0,
        0, 0,
        20, 50,
        0.05f, 0.2f,
        ORANGE,
    };
    particleManager.CreateSpawner(20, 0.2f, params);
    
    // Shoot a new cannonball.
    projectiles.push_back(new CannonBall(particleManager, shootingPoint, Maths::Vector2(transform.rotation, projectileVelocity, true), airTime, groundHeight));
    projectiles.back()->applyDrag = applyDrag;
    projectiles.back()->radius    = properties.projectileRadius;
    projectiles.back()->mass      = properties.projectileMass;

    ApplyRecoil();

    // Destroy the oldest projectile if there are too many.
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
