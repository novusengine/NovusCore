#pragma once
#include "../NovusTypes.h"
#include <chrono>

class Timer
{
public:

	Timer();
	~Timer();

	void Tick();
	void Reset();

	f32 GetLifeTime();
	f32 GetDeltaTime();

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _startTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> _lastTick;
};
