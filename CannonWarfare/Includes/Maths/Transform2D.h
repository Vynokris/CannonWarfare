#pragma once
#include "Vector2.h"

namespace Maths
{
    struct Transform2D
    {
        Vector2 position;
        Vector2 velocity;
        Vector2 acceleration;
        float   rotation;
        float   angularVelocity;
        bool    rotateForwards = false;

        void Update(const float& deltaTime);
    };
}
