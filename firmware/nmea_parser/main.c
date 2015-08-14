#include <nmea.h>

/*
$GPVTG,113.81,T,,M,0.35,N,0.64,K,A*33
$GPGGA,090712.000,5553.2371,N,03739.1067,E,1,10,0.90,182.9,M,14.4,M,,*65
$GNGSA,A,3,25,06,02,12,24,14,29,,,,,,1.16,0.90,0.73*1F
$GNGSA,A,3,87,86,70,,,,,,,,,,1.16,0.90,0.73*11
$GPGSV,3,1,09,12,69,130,33,25,67,284,24,02,58,111,38,06,38,058,33*77
$GPGSV,3,2,09,29,34,245,16,31,24,317,20,24,15,184,31,33,11,238,*7E
$GPGSV,3,3,09,14,09,266,25*49
$GLGSV,3,1,09,78,68,288,,87,66,048,19,88,58,233,,77,43,174,*6C
$GLGSV,3,2,09,79,25,325,,71,17,050,,81,17,231,,70,15,359,21*65
$GLGSV,3,3,09,86,11,051,31*54
$GNRMC,090712.000,A,5553.2371,N,03739.1067,E,0.31,113.81,110815,,,A*76
$GPVTG,113.81,T,,M,0.31,N,0.58,K,A*38
$GPGGA,090713.000,5553.2371,N,03739.1066,E,1,10,0.90,182.9,M,14.4,M,,*65
$GNGSA,A,3,25,06,02,12,24,14,29,,,,,,1.16,0.90,0.73*1F
$GNGSA,A,3,87,86,70,,,,,,,,,,1.16,0.90,0.73*11
$GPGSV,3,1,09,12,69,130,33,25,67,284,22,02,58,111,38,06,38,058,33*71
$GPGSV,3,2,09,29,34,245,16,31,24,317,20,24,15,184,31,33,11,238,*7E
$GPGSV,3,3,09,14,09,266,25*49
$GLGSV,3,1,09,78,68,288,,87,66,048,19,88,58,233,,77,43,174,*6C
$GLGSV,3,2,09,79,25,325,,71,17,050,,81,17,231,,70,15,359,22*66
$GLGSV,3,3,09,86,11,051,31*54
$GNRMC,090713.000,A,5553.2371,N,03739.1066,E,0.31,113.81,110815,,,A*76
$GPVTG,113.81,T,,M,0.31,N,0.58,K,A*38
 */

void ( *fillNMEActx ) ( int, const char* );
nmeactx nmea;

#define NMEA_MAX_LEN 82


typedef enum NMEATALKER
{
	GP=1, GL, GN
} NMEATALKER;

typedef enum NMEATYPE
{
	GGA=1, GSV, VTG, RMC, GSA, WRONG
} NMEATYPE;

const char nmeatempl;


typedef struct NMEASTRUCT
{
	NMEATYPE type;
	const char* str;
} NMEASTRUCT;

const NMEASTRUCT nmeatypesstr[] =
{
	{.type=GGA, .str="GGA"},
	{.type=GSV, .str="GSV"},
	{.type=VTG, .str="VTG"},
	{.type=RMC, .str="RMC"},
	{.type=GSA, .str="GSA"},
	{.type=0, .str=NULL},
};

const NMEASTRUCT nmeatalkerstr[] =
{
	{.type=GP, .str="GP"},
	{.type=GN, .str="GN"},
	{.type=GL, .str="GL"},
	{.type=0, .str=NULL},
};

NMEATYPE get_nmea_sent_type ( const char* field )
{
	for ( int i=0; nmeatypesstr[i].str; ++i )
		if ( !strncmp ( field+3, nmeatypesstr[i].str, 3 ) )
			return nmeatypesstr[i].type;
	return WRONG;
}

NMEATALKER get_nmea_talker ( const char* field )
{
	for ( int i=0; nmeatypesstr[i].str; ++i )
		if ( !strncmp ( field+1, nmeatypesstr[i].str,2 ) )
			return nmeatypesstr[i].type;
	return WRONG;
}


//by Fallout [FXD team]
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
		if ( ( *str >= '0' ) && ( *str <= '9' ) )
		{
			res = ( res * 10 ) + ( ( *str ) - '0' );
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

bool ckecknmea ( uint8_t sum, char* string )
{
	return str16_to_uint ( string ) == sum;
}

void nmeactxreset ( void )
{
	nmea.fp = nmea.fstr;
	nmea.sect = 0;
	nmea.sumdone = false;
	nmea.nmeaok = false;
	nmea.nmeaerr = 0;
	nmea.checksum = 0;
}

/*void printPS ( void )
{
	printf ( "Checksum: %s\n", nmea.nmeaok ? "OK":"ERROR" );
	printf ( "UTC:%u\n", nmea.utc );
	printf ( "LAT: %.3u.%.2u\'%.2u\" %c\n", nmea.lat.deg, nmea.lat.min, nmea.lat.sec, nmea.lat.dir );
	printf ( "LON: %.3u.%.2u\'%.2u\" %c\n", nmea.lon.deg, nmea.lon.min, nmea.lon.sec, nmea.lon.dir );
	printf ( "KMH: %.2f\n", nmea.kmh );
	printf ( "KTS: %.2f\n", nmea.knots );
}*/

void parseNMEA ( char c )
{
	if ( nmea.nmeaerr && '$' != c )
		return;
	switch ( c )
	{
		case '$':
			nmeactxreset();
			*nmea.fp++ = c;
			break;
		case '*':
			nmea.sumdone=true;
			nmea.sect++;
			memset ( nmea.fstr, 0, sizeof nmea.fstr );
			nmea.fp = nmea.fstr;
			break;
		case '\n':
			break;
		case '\r':
			//printf("Calculated sum: %X [%s]\n", checksum, fstr);
			nmea.nmeaok = ckecknmea ( nmea.checksum, nmea.fstr );
			//printPS();
			nmea.checksum = 0;
			break;
		case ',':
			nmea.checksum ^= c;
			//printf("char: \"%c\", [%X]\n",c , checksum);
			if ( 0 == nmea.sect )
			{
				switch ( get_nmea_sent_type ( nmea.fstr ) )
				{
					case GGA:
						//puts ( "GGA string" );
						fillNMEActx = &fillGGActx;
						break;
					case VTG:
						//puts ( "VTG string" );
						fillNMEActx = &fillVTGctx;
						break;
					case RMC:
						//puts ( "RMC string" );
						fillNMEActx = &fillRMCctx;
						break;
				}
			}
			fillNMEActx ( nmea.sect, nmea.fstr );
			nmea.sect++;
			memset ( nmea.fstr, 0, sizeof nmea.fstr );
			nmea.fp = nmea.fstr;
			break;
		default:
			*nmea.fp++ = c;
			if ( !nmea.sumdone )
			{
				//printf ("%c", c);
				//printf("char: \"%c\", [%X]\n",c , checksum);
				nmea.checksum ^= c;
			}
	}
}

/*int main ( void )
{
	static const char* nmeastr =
	"$GPVTG,113.81,T,,M,0.31,N,0.58,K,A*38\r\n\
	$GPGGA,090712.000,5553.2371,N,03739.1067,E,1,10,0.90,182.9,M,14.4,M,,*65\r\n\
	$GNRMC,090713.000,A,5553.2371,N,03739.1066,E,0.31,113.81,110815,,,A*76\r\n";
	char const* p = nmeastr;
	while ( *p )
	{
		parseNMEA ( *p++ );
	}
}*/
