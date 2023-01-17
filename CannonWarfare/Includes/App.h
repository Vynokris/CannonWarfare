#pragma once
#include "Cannon.h"
#include "Color.h"
#include <chrono>

class App
{
private:
	Maths::Vector2 screenSize;
	Maths::RGBA    clearColor;
	int   targetFPS;
	float targetDeltaTime;

	Cannon cannon;
	float  groundHeight;

	void DrawUi();

public:
	static inline std::chrono::system_clock::time_point startTime;
	static float GetTimeSinceStart();

	App(const Maths::Vector2& _screenSize, const int& _targetFPS, const Maths::RGBA& _clearColor = {});
	~App();

	void Update(const float& deltaTime);
	void Draw();

	int            GetScreenWidth    () { return (int)screenSize.x; }
	int            GetScreenHeight   () { return (int)screenSize.x; }
	Maths::Vector2 GetScreenSize     () { return screenSize;        }
	int            GetTargetFPS      () { return targetFPS;         }
	float          GetTargetDeltaTime() { return targetDeltaTime;   }
};