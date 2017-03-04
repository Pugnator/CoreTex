#include <global.hpp>
#include <common.hpp>
#include <core/io_macro.hpp>
#include <core/isr_helper.hpp>
#include <core/stm32f10x.hpp>
#include <drivers/gps.hpp>
#include <log.hpp>
#include <string.h>

#define RTT_DEBUG_CHANNEL 0

const NMEATYPESTRUCT nmeatypesstr[] =
{
{ GGA, "GGA" },
{ GSV, "GSV" },
{ VTG, "VTG" },
{ RMC, "RMC" },
{ GSA, "GSA" },
{ (NMEATYPE) 0, NULL }, };

const NMEATALKERSTRUCT nmeatalkerstr[] =
{
{ GP, "GP" },
{ GN, "GN" },
{ GL, "GL" },
{ (NMEATALKER) 0, NULL }, };

/* Pointer to the Gps object itself in order to be accessible from within a static method */
class Gps *Gps::self = nullptr;

void
Gps::gpsisr (void)
{
	if (self->Reg->SR & USART_SR_RXNE)
	{
		short ch = self->Reg->DR;
		self->Reg->SR &= ~USART_SR_RXNE;
		if (self->ready)
		{
			return;
		}

		if (0 == self->nmeastr_len && '$' != ch)
		{
			return;
		}
		else if ('\n' == ch || NMEA_MAX_LEN <= self->nmeastr_len + 1)
		{
			self->nmeastr[self->nmeastr_len] = ch;
			self->ready = true;
			return;
		}
		self->nmeastr[self->nmeastr_len++] = ch;
	}
}

NMEATYPE
Gps::get_nmea_sent_type (const char* field)
{
	for (int i = 0; nmeatypesstr[i].str; ++i)
		if (!strncmp (field + 3, nmeatypesstr[i].str, 3))
		{
			return nmeatypesstr[i].type;
		}
	SEGGER_RTT_printf (0, "Sender type '%s'\r\n", field);
	return WRONG;
}

NMEATALKER
Gps::get_nmea_talker (const char* field)
{
	if (!strncmp (field + 1, "PMTK", 4))
	{
		return PMTK;
	}
	for (int i = 0; nmeatalkerstr[i].str; ++i)
	{
		if (!strncmp (field + 1, nmeatalkerstr[i].str, 2))
		{
			return nmeatalkerstr[i].type;
		}
	}
	return GP;
}

void
Gps::latlon2crd (const char* str, coord* c)
{
	c->valid = false;
	if(strlen(str) < 7)
	{
		return;
	}
	//Check for valid string here
	//XXXYY.ZZ or XXYY.ZZ
	char latlon[9] =
	{ 0 };
	strcpy (latlon, str);
	char* p = latlon;
	while (*p++ != '.')
		;
	c->sec = str10_to_word (p) / 60;
	*--p = 0;
	c->min = str10_to_word (p - 2);
	p -= 2;
	*p = 0;
	c->deg = str10_to_word (latlon);
	c->valid = true;
}

bool
Gps::ckecknmea (uint8_t sum, char* string)
{
	return str16_to_word (string) == sum;
}

void
Gps::reset (void)
{
	nmea.sect = 0;
	nmea.sumdone = false;
	nmea.nmeaok = false;
	nmea.nmeaerr = 0;
	nmea.checksum = 0;
	nmea.lat.valid = false;
	nmea.lat.valid = false;
	nmeastr_len = 0;
	nmea.fp = nmea.fstr;
	memset (nmeastr, 0, NMEA_MAX_LEN + 1);
	correct = false;
	ready = false;
}

NMEAERR
Gps::parseNMEA (char c)
{
	if (nmea.nmeaerr && '$' != c)
	{
		return NMEA_NOT_BEGINNING;
	}
	switch (c)
	{
		case '$':
			*nmea.fp++ = c;
			break;
		case '*':
			nmea.sumdone = true;
			nmea.sect++;
			memset (nmea.fstr, 0, sizeof nmea.fstr);
			nmea.fp = nmea.fstr;
			break;
		case '\n':
			break;
		case '\r':
			nmea.nmeaok = ckecknmea (nmea.checksum, nmea.fstr);
			break;
		case ',':
			nmea.checksum ^= c;
			if (0 == nmea.sect)
			{
				if (PMTK == get_nmea_talker (nmea.fstr))
				{
					return NMEA_DIAGNOSTIC_MSG;
				}
				type = get_nmea_sent_type (nmea.fstr);
			}
			//TODO: make some kind of specialized template here based on NMEATYPE
			if (*nmea.fstr)
			{
				switch (type)
				{
					case GGA:
						fillGGActx (nmea.sect, nmea.fstr);
						break;
					case VTG:
						fillVTGctx (nmea.sect, nmea.fstr);
						break;
					case RMC:
						fillRMCctx (nmea.sect, nmea.fstr);
						break;
					default:
						return NMEA_UNKNOWN_TALKER;
						break;
				}
			}
			nmea.sect++;
			memset (nmea.fstr, 0, sizeof nmea.fstr);
			nmea.fp = nmea.fstr;
			break;
		default:
			*nmea.fp++ = c;
			if (!nmea.sumdone)
			{
				nmea.checksum ^= c;
			}
	}
	return NMEA_ERROR_OK;
}

void
Gps::rttprint ()
{
	if (!nmea.nmeaok)
	{
		if (NMEA_ERROR_OK != prepare ())
		{
			return;
		}
	}
	SEGGER_RTT_WriteString (0, nmeastr);
	SEGGER_RTT_printf (0, "Checksum [%X]: %s\n", nmea.checksum,
	                   nmea.nmeaok ? "OK" : "ERROR");
	SEGGER_RTT_printf (0, "UTC: %6u\n", nmea.utc);
	SEGGER_RTT_printf (0, "LAT:%3u.%2u\'%2u\" %c\n", nmea.lat.deg, nmea.lat.min,
	                   nmea.lat.sec, nmea.lat.dir);
	SEGGER_RTT_printf (0, "LON:%3u.%2u\'%2u\" %c\n", nmea.lon.deg, nmea.lon.min,
	                   nmea.lon.sec, nmea.lon.dir);
	reset ();
}

NMEAERR
Gps::prepare (void)
{
	if (!ready)
	{
		return NMEA_NOT_READY;
	}
	NMEAERR err = NMEA_ERROR_OK;
	//SEGGER_RTT_printf(0, "NMEA: %s\r\n", nmeastr);
	for (int i = 0; i < nmeastr_len; ++i)
	{
		err = parseNMEA (nmeastr[i]);
		if (NMEA_ERROR_OK != err)
		{
			reset ();
			return err;
		}
	}
	correct = true;
	return err;
}

coord
Gps::getlat ()
{
	return nmea.lat;
}

coord
Gps::getlon ()
{
	return nmea.lon;
}

word
Gps::get_utc ()
{
	return nmea.utc;
}

double
Gps::get_dec_lat ()
{
	return nmea.lat.deg + (nmea.lat.min / 60) + (nmea.lat.sec / 3600);
}

double
Gps::get_dec_lon ()
{
	return nmea.lon.deg + (nmea.lon.min / 60) + (nmea.lon.sec / 3600);
}

UTM
Gps::coord2utm (coord c)
{
	UTM result;
	int nDigits = floor (log10 (abs (c.sec))) + 1;
	double power = pow (10, nDigits);
	double min = c.min + c.sec / power;
	min /= 60;
	double pos = c.deg + min;
	result.deg = (word) pos;
	result.fract = (word) (pos * 1000000UL);
	result.fract = result.fract - (result.deg * 1000000UL);
	return result;
}

bool Gps::ok()
{
	return nmea.lat.valid && nmea.lon.valid;
}
