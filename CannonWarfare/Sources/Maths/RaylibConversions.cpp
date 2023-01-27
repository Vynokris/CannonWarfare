#include "RaylibConversions.h"

::Vector2 ToRayVector2(const Maths::Vector2& vec) 
{ 
	return ::Vector2{ vec.x, vec.y }; 
}

Maths::Vector2 FromRayVector2(const Vector2& vec)
{
	return Maths::Vector2{ vec.x, vec.y }; 
}

Color ToRayColor(const Maths::RGBA& col) 
{ 
	return Color{ (unsigned char)(col.r * 255), (unsigned char)(col.g * 255), (unsigned char)(col.b * 255), (unsigned char)(col.a * 255) }; 
}

Color ToRayColor(const Maths::RGB& col) 
{ 
	return Color{ (unsigned char)(col.r * 255), (unsigned char)(col.g * 255), (unsigned char)(col.b * 255), 255 }; 
}
