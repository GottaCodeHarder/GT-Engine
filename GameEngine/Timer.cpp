// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Stop();
}

// ---------------------------------------------
void Timer::Start()
{
	if (pausedAt == 0.f)
		startedAt = SDL_GetTicks();
	else
		startedAt += SDL_GetTicks() - pausedAt;
	
	pausedAt = 0.f;
}

// ---------------------------------------------
void Timer::Pause()
{
	if(pausedAt == 0.f)
		pausedAt = SDL_GetTicks();
}

// ---------------------------------------------
void Timer::Stop()
{
	startedAt = pausedAt = 0.f;
	memory = NULL;
}

// ---------------------------------------------
Uint32 Timer::Read() const
{
	if (pausedAt == 0.f)
		return (SDL_GetTicks() - startedAt);
	else
		return (pausedAt - startedAt);
}

float Timer::readSec() const
{
	return (float)Read() / 1000.0f;
}

bool Timer::SoundsTimer()
{
	if (SDL_GetTicks() / 1000 - memory == 1)
	{
		memory = SDL_GetTicks() / 1000;
		return true;
	}
	return false;
}

void Timer::ResetTimer()
{
	startedAt = 0.f;
	pausedAt = 0.f;
	timeStartSeconds = 0.f;
	timeStartMinutes = 0.f;
}

Uint32 Timer::Chronometer(bool set, int pos)
{
	if (!set)
	{
		timeStartSeconds = (SDL_GetTicks() / 1000) % 60;
		timeStartMinutes = (SDL_GetTicks() / 1000) / 60;
	}
	switch (pos)
	{
	case 2:
		return ((SDL_GetTicks() / 1000) % 60) - timeStartSeconds;
	case 3:
		return ((SDL_GetTicks() / 1000) / 60) - timeStartMinutes;
	default:
		return NULL;
	}
}