/*1 - +3.3
2 - VBAT
3 - GND
4 - TX (out)
5 - RX (in)
6 - GND*/
#include <global.hpp>

#define NMEA_MAX_LEN 82
nmeactx nmea;

static int sect = 0;
static char fstr[16] = {0};
static char* fp = NULL;
static uint16_t checksum = 0;
static bool sumdone = false;
static bool nmeaok = false;
static bool start = false;

typedef enum GGAF
{
	GPGGA  = 0, UTC, LAT, LATDIR, LON, LONGDIR, QUAL, SATNUM, HDOP, MSL, ALT, GEOSEP, GEOSEPU, AGE, ID, CS
} GGAF;

void latlon2crd ( const char* str, coord* c )
{
	//Check for valid string here
	//XXXYY.ZZ or XXYY.ZZ
	char latlon[9] = {0};
	strcpy ( latlon, str );
	char* p = latlon;
	while ( *p++ != '.' );
	c->sec = str10_to_uint ( p ) / 60;
	*--p = 0;
	c->min = str10_to_uint ( p-2 );
	p -= 2;
	*p = 0;
	c->deg = str10_to_uint ( latlon );
}

bool ckecknmea ( uint16_t sum, char* string )
{
	return str16_to_uint ( string ) == sum;
}

void fillNMEActx ( int sect, const char* field )
{
	switch ( sect )
	{
		case UTC:
			nmea.utc = str10_to_uint ( field );
			break;
		case LON:
			latlon2crd ( field, &nmea.lon );
			break;
		case LONGDIR:
			nmea.lon.dir = *field;
			break;
		case LAT:
			latlon2crd ( field, &nmea.lat );
			break;
		case LATDIR:
			nmea.lat.dir = *field;
			break;
		case MSL:
			nmea.msl = str10_to_uint ( field );
			break;
	}
}

void parseNMEA ( char c )
{
	if ( '$' != c || !start )
		return;
		
	switch ( c )
	{
	
		case '\n':
			start = false;
			break;
		case '$':
			start = true;
			sect = 0;
			sumdone = false;
			nmeaok = false;
			nmea.done = false;
			break;
		case '*':
			sumdone = true;
		case '\r':
			nmea.done = true;
		case ',':
			if ( sumdone && '\r' == c )
			{
				nmeaok = ckecknmea ( checksum, fstr );
			}
			fillNMEActx ( sect, fstr );
			sect++;
			memset ( fstr, 0, sizeof fstr );
			fp = fstr;
			break;
		default:
			*fp++ = c;
			if ( !sumdone )
				checksum ^= c;
	}
}