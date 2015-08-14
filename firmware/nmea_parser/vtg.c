#include <nmea.h>


typedef enum NMEAFORMAT
{
	TYPE , TDEGR, TTRUE, TDEGR2, MAGNETIC, SPEEDKN, KNOTS, SPEEDKM, KMH, CS, END
} NMEAFORMAT;

void fillVTGctx ( int sect, const char* field )
{
	switch ( sect )
	{
		case SPEEDKN:
			nmea.knots = strtod ( field, NULL );
			break;
		case SPEEDKM:
			nmea.kmh = strtod ( field, NULL );
			break;
		case END:
			break;
	}
}