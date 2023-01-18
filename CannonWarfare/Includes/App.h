#pragma once
#include "Cannon.h"
#include "Star.h"
#include "ParticleManager.h"
#include <chrono>

constexpr size_t STAR_COUNT = 100;
class Graphics;

class App
{
private:
	static inline std::chrono::system_clock::time_point startTime;
	
	Maths::Vector2  screenSize;
	int             targetFPS;
	float           targetDeltaTime;
	Graphics*       graphics;
	ParticleManager particleManager;

	std::vector<Star> stars;
	Cannon cannon;
	float  groundHeight;

	void DrawUi();

public:

	App(const Maths::Vector2& _screenSize, const int& _targetFPS);
	~App();

	void Update(const float& deltaTime);
	void Draw();

	int            GetScreenWidth    () const { return (int)screenSize.x; }
	int            GetScreenHeight   () const { return (int)screenSize.x; }
	Maths::Vector2 GetScreenSize     () const { return screenSize;        }
	int            GetTargetFPS      () const { return targetFPS;         }
	float          GetTargetDeltaTime() const { return targetDeltaTime;   }
	
	static float GetTimeSinceStart();
	
	ParticleManager& GetParticleManager() { return particleManager; } 
};
