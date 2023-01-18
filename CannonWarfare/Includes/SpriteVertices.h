#pragma once

#include "Vector2.h"
#include <vector>

class Sprite
{
private:
    Maths::Vector2* verticies;
    int* lines;

public:
    Sprite(Maths::Vector2* _verticies, int* _lines) : verticies(_verticies), lines(_lines) {}
    ~Sprite() {}
};

static class Sprites
{
public:
    static inline Sprite testSprite = Sprite(
        {
            new Maths::Vector2[] 
            {
                Maths::Vector2(-1.0f, 0.0f),
                Maths::Vector2(-0.0f, -1.0f),
                Maths::Vector2(1.0f, 0.0f),
                Maths::Vector2(-0.0f, 1.0f)
            },
            new int[]
            {
                0,1, 
                1,2, 
                2,3, 
                3,0
            }
        }
    );
};