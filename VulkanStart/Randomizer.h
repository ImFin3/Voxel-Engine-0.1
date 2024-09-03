#ifndef RANDOMIZER_H

#define RANDOMIZER_H

#include <cstdlib>

static class Randomizer 
{
public:
	static float RandomIntAsFloatBetween(const int& start, const int& end);
	static float RandomFloatBetween01();
};
#endif // !RANDOMIZER_H
