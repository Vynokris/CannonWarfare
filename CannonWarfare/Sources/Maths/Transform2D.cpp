#include "Transform2D.h"

void Maths::Transform2D::Update(const float& deltaTime)
{
    if (rotateForwards)
        rotation = velocity.GetAngle();
    else
        rotation += angularVelocity * deltaTime;
    
    velocity += acceleration * deltaTime;
    position += velocity     * deltaTime;
}
