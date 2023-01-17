#pragma once

#include "raylib.h"
#include "Maths/Vector2.h"
#include <vector>

class Particle
{
public:
	enum class Shape {
		LINE,
		SQUARE,
		CIRCLE,
	};

private:
	Shape shape;
	
	Maths::Vector2 position;
	Maths::Vector2 velocity;

	float rotation;
	float size;
	float acceleration;
	float friction;

	Color color;

public:
	Particle(const Shape& _shape, const Maths::Vector2& _position, const Maths::Vector2& _velocity,
			 const float& _rotation, const float& _size, const float& _acceleration, const float& _friction, const Color& _color);
	~Particle();

	void SetVelocity(const Maths::Vector2& _velocity) { velocity = _velocity; }
	void SetPosition(const Maths::Vector2& _position) { position = _position; }
	void SetSize(const float& _size)				  { size	 = _size;	  }

	Maths::Vector2 GetPosition() const { return position; }
	Maths::Vector2 GetVelocity() const { return velocity; }

	float GetAcceleration() const { return acceleration; }
	float GetFriction()		const { return friction;	 }
	float GetRotation()		const { return rotation;	 }
	float GetSize()			const { return size;		 }

	Color GetColor() const { return color; }
	Shape GetShape() const { return shape; }
};