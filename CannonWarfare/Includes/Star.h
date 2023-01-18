#pragma once
#include "Vector2.h"
#include "raylib.h"

class Star
{
private:
    const Maths::Vector2& screenSize;
    
public:
    Maths::Vector2 position;
    Maths::Vector2 velocity;
    int   radius = 0;
    Color color  = {};

    Star(const Maths::Vector2& _screenSize);

    void Update(const float& deltaTime);
    void Draw() const;
};
