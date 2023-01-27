#pragma once
#include "Maths.h"
#include "raylib.h"


::Vector2 ToRayVector2(const Maths::Vector2& vec);
Maths::Vector2 FromRayVector2(const ::Vector2& vec);

Color ToRayColor(const Maths::RGBA& col);
Color ToRayColor(const Maths::RGB& col);
