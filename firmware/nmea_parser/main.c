#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

static const char* nmeastr =
	"$GPGGA,093632.873,7639.937,S,17925.800,E,0,00,,,M,,M,,*59\r\n";

void parseNMEA ( char c );

#define NMEA_MAX_LEN 82

int sect = 0;
char fstr[16] = {0};
char* fp = NULL;
uint16_t checksum = 0;
bool sumdone = false;
bool nmeaok = false;


typedef enum GGAF
{
    GPGGA , UTC, LAT, LATDIR, LONG, LONGDIR, QUAL, SATNUM, HDOP, MSL, ALT, GEOSEP, GEOSEPU, AGE, ID, CS
} GGAF;

//From http://stackoverflow.com/questions/19569965/hexadecimal-string-conversion-to-integer-in-c
uint32_t str16_to_uint ( char const* str )
{
	uint32_t res = 0;
	char c = NULL;
	while ( *str )
	{
		c = *str;
		res <<= 4;
		res += ( c > '9' ) ? ( c & 0xDFu ) - 0x37u: ( c - '0' );
		++str;
	}
	return res;
}

uint32_t str10_to_uint ( char const* str )
{
	uint32_t res = 0;
	while ( *str )
	{
		if ((*str >= '0') && (*str <= '9'))
    	{
      		res = (res * 10) + ((*str) - '0');
    	}
    	else
    	{
    		//error here!
    		return 0;
    	}
    	str++;
	}
	return res;
}

const char* fieldstr[] =
{
	"Type",
	"UTC",
	"LAT",
	"LATDIR",
	"LONG",
	"LONGDIR",
	"QUAL",
	"SATNUM",
	"HDOP",
	"MSL",
	"ALT",
	"GEOSEP",
	"GEOSEPU",
	"AGE",
	"ID",
	"CS",
	NULL
};

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
}nmeactx;

nmeactx nmea;

void latlon2crd (const char *str, coord *c)
{
	//Check for valid string here
	//XXXYY.ZZ or XXYY.ZZ
	char latlon[9] = {0};
	strcpy(latlon, str);
	char *p = latlon;
	while(*p++ != '.');
	c->sec = str10_to_uint(p) / 60;
	*--p = 0;
	c->min = str10_to_uint(p-2);
	p -= 2;
	*p = 0;
	c->deg = str10_to_uint(latlon);
}

const char* field2str ( int f )
{
	return fieldstr[f];
}

bool ckecknmea ( uint16_t sum, char* string )
{
	return str16_to_uint ( string ) == sum;
}

void fillNMEActx (int sect, const char *field)
{
	switch (sect)
	{
		case UTC:
		nmea.utc = str10_to_uint(field);
		break;
		case LONG:
		latlon2crd(field, &nmea.lon);		
		break;
		case LONGDIR:
		nmea.lon.dir = *field;
		break;
		case LAT:
		latlon2crd(field, &nmea.lat);		
		break;
		case LATDIR:
		nmea.lat.dir = *field;
		break;
		case MSL:
		nmea.msl = str10_to_uint(field);
		break;
	}
}

void parseNMEA ( char c )
{
	switch ( c )
	{
	
		case '\n':
			break;
		case '$':
			sect = 0;
			sumdone = false;
			nmeaok = false;
			break;
		case '*':
			sumdone = true;
		case '\r':
		case ',':
			if ( sumdone && '\r' == c )
			{
				nmeaok = ckecknmea ( checksum, fstr );
			}
			fillNMEActx(sect, fstr);
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

int main ( void )
{
	fp = fstr;
	char const* p = nmeastr;
	while ( *p )
	{
		parseNMEA ( *p );
		p++;
	}
	printf ( "Checksum: %s\n", nmeaok ? "OK":"ERROR" );
	printf("GPS data:\n");
	printf("LAT: %.3u.%.2u\'%.2u\" %c\n", nmea.lat.deg, nmea.lat.min, nmea.lat.sec, nmea.lat.dir);
	printf("LON: %.3u.%.2u\'%.2u\" %c\n", nmea.lon.deg, nmea.lon.min, nmea.lon.sec, nmea.lon.dir);
}
