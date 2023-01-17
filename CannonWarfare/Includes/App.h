#pragma once
#include "Vector2.h"

class App
{
private:
	Maths::Vector2 screenSize;
	int   targetFPS;
	float targetDeltaTime;

public:
	App(const Maths::Vector2& _screenSize, const int& _targetFPS);
	~App();

	void Update(const float& deltaTime);
	void Draw();

	int            GetScreenWidth    () { return (int)screenSize.x; }
	int            GetScreenHeight   () { return (int)screenSize.x; }
	Maths::Vector2 GetScreenSize     () { return screenSize;        }
	int            GetTargetFPS      () { return targetFPS;         }
	float          GetTargetDeltaTime() { return targetDeltaTime;   }
};