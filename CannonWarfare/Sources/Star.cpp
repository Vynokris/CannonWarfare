#include "Star.h"
#include "Arithmetic.h"
using namespace Maths;

Star::Star(const Maths::Vector2& _screenSize)
    : screenSize(_screenSize)
{
    // Get a random position and radius for the star.
    position = { (float)(rand() % (int)screenSize.x), (float)(rand() % (int)screenSize.y) };
    radius   = rand() % 5 - 1; if (radius <= 0) radius = 1;
    velocity = { -20.f * radius, 0 };

    // Get random red green and blue values.
    float R = (rand() % 120 + 135) / 255.0f;
    float G = (rand() % 120 + 135) / 255.0f;
    float B = (rand() % 120 + 135) / 255.0f;

    // Make the color as white as possible.
    const float minVal = min(1-R, min(1-G, 1-B));
    R += minVal;
    G += minVal;
    B += minVal;

    // Set the star's color.
    color = { (unsigned char)(R * 255), (unsigned char)(G * 255), (unsigned char)(B * 255), 255 };
}

void Star::Update(const float& deltaTime)
{
    // Move the star according to its velocity.
    position += velocity * deltaTime;

    // Screen wrapping.
    if (position.x < 0.f)
        position.x += screenSize.x;
}

void Star::Draw() const
{
    DrawCircle((int)position.x, (int)position.y, (float)radius, color);
}
