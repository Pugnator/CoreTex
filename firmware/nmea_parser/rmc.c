#include <nmea.h>

/*
RMC
Recommended Minimum Navigation Information

$--RMC,hhmmss.ss,A,llll.ll,a,yyyyy.yy,a,x.x,x.x,xxxx,x.x,a*hh

Time (UTC)
Status, V = Navigation receiver warning
Latitude
N or S
Longitude
E or W
Speed over ground, knots
Track made good, degrees true
Date, ddmmyy
Magnetic Variation, degrees
E or W
Checksum
*/

typedef enum NMEAFORMAT
{
	TYPE , UTC, STATUS, LAT, LATDIR, LONG, LONGDIR, KNOTS, TMG, DATE, CS, LONGDIR2, END
} NMEAFORMAT;

void fillRMCctx ( int sect, const char* field )
{
	switch ( sect )
	{
		case UTC:
			*strstr ( field, "." ) = 0;
			nmea.utc = str10_to_uint ( field );
			break;
		case LONG:
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
		case KNOTS:
			nmea.knots = strtod ( field, NULL );
			break;
		case END:
			break;
	}
}