#ifndef __GTTIMER_H__
#define __GTTIMER_H__

#include "../SDL/include/SDL.h"

class GTITimer
{
public:
	GTITimer() {};

	void Start() {	startedAt = SDL_GetTicks(); };

	Uint32 Read() const { return (SDL_GetTicks() - startedAt); };

private:
	Uint32 startedAt = 0;
};

#endif