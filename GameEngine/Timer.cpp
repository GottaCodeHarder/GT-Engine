// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	Start();
}

// ---------------------------------------------
void Timer::Start()
{
	memory = NULL;
	running = true;
	started_at = (SDL_GetTicks());
}

// ---------------------------------------------
void Timer::Stop()
{
	running = false;
	stopped_at = (SDL_GetTicks() );
}

// ---------------------------------------------
Uint32 Timer::Read()
{
	if(running == true)
	{
		return ((SDL_GetTicks() ) - started_at);
	}
	else
	{
		return stopped_at - started_at;
	}
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