#include "Random.h"
#include "MathGeoLib\Time\Clock.h"

Random::Random()
{
	
}

float Random::RndFloat()
{
	Algorithm.Seed(Clock::TickU32()); // Seed	

	return Algorithm.Float();
}

float Random::RndFloat(float min, float max)
{
	Algorithm.Seed(Clock::TickU32()); // Seed	

	return Algorithm.Float(min, max);
}

int Random::RndInt(int min, int max)
{
	Algorithm.Seed(Clock::TickU32()); // Seed	

	return Algorithm.Int(min, max);
}