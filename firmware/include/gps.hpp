#pragma once
#include <global.hpp>

typedef struct coord
{
	uint32_t deg;
	uint32_t min;
	uint32_t sec;
	char dir;
} coord;

typedef struct nmeactx
{
	uint32_t utc;
	coord lat;
	coord lon;
	uint32_t msl;
	bool done;
} nmeactx;

extern nmeactx nmea;

void parseNMEA ( char c );