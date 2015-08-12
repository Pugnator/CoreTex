#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"

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
}nmeactx;

uint32_t str16_to_uint ( char const* str );
uint32_t str10_to_uint ( char const* str );
void latlon2crd (const char *str, coord *c);
bool ckecknmea ( uint8_t sum, char* string );

extern nmeactx nmea;
extern int nmeaerr;

#include <gga.h>
#include <vtg.h>