#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:

	enum class TimerState
	{
		PLAY,
		PLAY_PAUSE,
		STOP
	};

	// Constructor
	Timer();

	void Start(bool toggle = false);
	void Stop(bool toggle = false);

	bool SoundsTimer(); 
	void ResetTimer();

	Uint32 Chronometer(bool set, int pos); // Sets/Returns started match time. Set = Is Set? Pos = Min/Sec/Milisec
	Uint32 Read() const;

	float readSec() const;

	TimerState GetTimerState() const;

private:

	TimerState state;

	int		memory;
	Uint32	startedAt;
	Uint32	stoppedAt;
	Uint32	timeStartSeconds;
	Uint32	timeStartMinutes;
};

#endif //__TIMER_H__