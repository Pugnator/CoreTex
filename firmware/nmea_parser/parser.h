#ifdef __cplusplus
extern "C" {
#endif

#pragma once
#include <stdint.h>

typedef struct coord
{
	uint32_t deg;
	uint32_t min;
	uint32_t sec;
	char dir;
}coord;

typedef struct nmeactx
{
	uint32_t utc;
	coord lat;
	coord lon;
	uint32_t msl;
	double knots;
	double kmh;
	float course;
	bool isvalid;
	int32_t nmeaerr;
	int32_t sect;
	char fstr[16];
	char* fp;
	uint8_t checksum;
	bool sumdone;
	bool nmeaok;
}nmeactx;

void parseNMEA ( char c ); 

#ifdef __cplusplus
}
#endif