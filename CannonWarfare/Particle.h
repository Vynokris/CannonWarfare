#pragma once

#include "raylib.h"
#include "Maths/Vector2.h"
#include <vector>

class Particle
{
public:
	enum class Shape {
		CIRCLE,
		LINE,
		SQUARE
	};

private:
	Maths::Vector2 position;
	Maths::Vector2 velocity;

	float rotation;
	float size;
	float friction;
	float acceleration;

	Color color;
	Shape shape;

public:
	Particle(const Maths::Vector2& _position, const float& _rotation, const float& _size, const Color& _color, const Maths::Vector2& _velocity, const float& _acceleration, const float& _friction, const Shape& _shape);
	~Particle();

	void setVelocity(const Maths::Vector2& _velocity) { velocity = _velocity; }
	void setPosition(const Maths::Vector2& _position) { position = _position; };
	void setSize(const float& _size)				  { size	 = _size;	  };

	Maths::Vector2 getPosition() const { return position; };
	Maths::Vector2 getVelocity() const { return velocity; };

	float getAcceleration() const { return acceleration; };
	float getFriction()		const { return friction;	 };
	float getRotation()		const { return rotation;	 };
	float getSize()			const { return size;		 };

	Color getColor() const { return color; };
	Shape getShape() const { return shape; };
};