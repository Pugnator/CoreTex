#pragma once
#include <stdint.h>

typedef struct coord
{
	uint deg;
	uint min;
	uint sec;
	char dir;
}coord;

typedef struct nmeactx
{
	uint utc;
	coord lat;
	coord lon;
	uint msl;
	double knots;
	double kmh;
	float course;
	bool isvalid;
	int nmeaerr;
	int sect;
	char fstr[16];
	char* fp;
	uint8_t checksum;
	bool sumdone;
	bool nmeaok;
}nmeactx;

void parseNMEA ( char c );
