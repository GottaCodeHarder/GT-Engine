// ----------------------------------------------------
// Timer.cpp
// Body for CPU Tick Timer class
// ----------------------------------------------------

#include "Timer.h"

// ---------------------------------------------
Timer::Timer()
{
	//Start();
	state = TimerState::STOP;
}

// ---------------------------------------------
void Timer::Start()
{
	if (state == TimerState::STOP)
	{
		startedAt = (SDL_GetTicks());
		state = TimerState::PLAY;
	}
}

void Timer::Continue()
{	
	startedAt = stoppedAt;
	state = TimerState::PLAY;
}

// ---------------------------------------------
void Timer::Stop()
{
	if (state == TimerState::PLAY)
		memory = NULL;

	state = TimerState::STOP;
	stoppedAt = (SDL_GetTicks());
}

// ---------------------------------------------
Uint32 Timer::Read() const
{
	if(state == TimerState::PLAY)
	{
		return ((SDL_GetTicks()) - startedAt);
	}
	else
	{
		return stoppedAt - startedAt;
	}
}

float Timer::readSec() const
{
	return (float)Read() / 1000.0f;
}

Timer::TimerState Timer::GetTimerState() const
{
	return state;
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
	stoppedAt = 0.f;
	timeStartMinutes = 0.f;
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