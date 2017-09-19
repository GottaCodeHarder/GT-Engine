#ifndef __RANDOM_H__
#define __RANDOM_H__

#include "Globals.h"
#include "MathGeoLib\Algorithm\Random\LCG.h"

class Random
{
public:
	Random();

	float RndFloat();						// Random float from 0 to 1
	float RndFloat(float min, float max);	// Random float in a range
	int RndInt(int min, int max);			// Random int in a range

private:
	math::LCG Algorithm;
};

#endif