#include <global.h>


typedef enum NMEAFORMAT
{
    TYPE , UTC, LAT, LATDIR, LONG, LONGDIR, QUAL, SATNUM, HDOP, MSL, ALT, GEOSEP, GEOSEPU, AGE, ID, CS, END
}NMEAFORMAT;

void fillGGActx (int sect, const char *field)
{
	switch (sect)
	{
		case UTC:
		*strstr(field, ".") = 0;
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
		case END:
		break;
	}
}
