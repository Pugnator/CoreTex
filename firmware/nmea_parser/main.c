#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/* $GPRMC
         * note: a siRF chipset will not support magnetic headers.
         * $GPRMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,ddmmyy,x.x,a*hh
         * ex: $GPRMC,230558.501,A,4543.8901,N,02112.7219,E,1.50,181.47,230213,,,A*66,
         *
         * WORDS:
         *  1    = UTC of position fix
         *  2    = Data status (V=navigation receiver warning)
         *  3    = Latitude of fix
         *  4    = N or S
         *  5    = Longitude of fix
         *  6    = E or W
         *  7    = Speed over ground in knots
         *  8    = Track made good in degrees True, Bearing This indicates the direction that the device is currently moving in,
         *       from 0 to 360, measured in azimuth.
         *  9    = UT date
         *  10   = Magnetic variation degrees (Easterly var. subtracts from true course)
         *  11   = E or W
         *  12   = Checksum
         */

static const char* nmeastr =
	"$GPRMC,183729,A,3907.356,N,12102.482,W,000.0,360.0,080301,015.5,E*6F\r\n\
$GPRMC,183731,A,3907.482,N,12102.436,W,000.0,360.0,080301,015.5,E*67\r\n";

#define NMEA_MAX_LEN 82

typedef enum RMCFIELD
{
	NONE = 0, GPRMC , UTC, LAT, LATDIR, LONG, LONGDIR, SPD, HEAD, DATE, MAGVAR, MAGVARDIR, MODE, END
} RMCFIELD;

RMCFIELD sect = NONE;

void parseNMEA ( char c )
{
	switch ( c )
	{
		case '$':
			sect = GPRMC;
			break;
		case '\n':
			if(END != ++sect)
			{
				printf("%d\n", sect);
			}
			break;
		case '\r':
			break;
		case ',':
			sect++;
			break;
		default:
			;
	}
}

int main ( void )
{
	char const* p = nmeastr;
	while ( *p )
	{
		parseNMEA ( *p );
		p++;
	}
}