#ifndef __TIMER_H__
#define __TIMER_H__

#include "Globals.h"
#include "SDL\include\SDL.h"

class Timer
{
public:
	Timer();

	void Start();
	void Pause();
	void Stop();

	bool SoundsTimer(); 
	void ResetTimer();

	Uint32 Chronometer(bool set, int pos); // Sets/Returns started match time. Set = Is Set? Pos = Min/Sec/Milisec
	Uint32 Read() const;

	float readSec() const;

private:

	int		memory;
	Uint32	startedAt;
	Uint32	pausedAt;
	Uint32	timeStartSeconds;
	Uint32	timeStartMinutes;
};

#endif //__TIMER_H__