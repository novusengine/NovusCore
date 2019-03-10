#include "Timer.h"

/** A high resolution clock from Chrono */
using Clock = std::chrono::high_resolution_clock;

Timer::Timer()
{
	_startTime = Clock::now();
	_lastTick = Clock::now();
	_lastTick = Clock::now();
}

Timer::~Timer()
{

}

void Timer::Tick()
{
	_lastTick = Clock::now();
}

void Timer::Reset()
{
	_lastTick = Clock::now();
	_startTime = Clock::now();
}

f32 Timer::GetLifeTime()
{
	std::chrono::time_point<Clock> now = Clock::now();
	std::chrono::duration<float> lifeTime = now - _startTime;
	return lifeTime.count();
}

f32 Timer::GetDeltaTime()
{
	std::chrono::time_point<Clock> now = Clock::now();
	std::chrono::duration<float> delta = now - _lastTick;
	return delta.count();
}
