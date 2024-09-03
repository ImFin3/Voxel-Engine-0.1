#include "Randomizer.h"

float Randomizer::RandomIntAsFloatBetween(const int& start, const int& end)
{
	float rnd = rand() % (end - start);

	rnd += start;

	return rnd;
}

float Randomizer::RandomFloatBetween01()
{
	float rnd = (double)rand() / RAND_MAX;
	return rnd;
}
