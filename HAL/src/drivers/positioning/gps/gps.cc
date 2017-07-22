#include <global.hpp>
#include <common.hpp>
#include <core/io_macro.hpp>
#include <core/isr_helper.hpp>
#include <core/stm32f10x.hpp>
#include <core/rtc.hpp>
#include <drivers/gps.hpp>
#include <log.hpp>
#include <math.h>
#include <string.h>

#ifdef GPS_DEBUG
#define DEBUG_LOG SEGGER_RTT_printf
#else
#define DEBUG_LOG(...)
#endif

const NMEATYPESTRUCT nmeatypesstr[] =
{
{ GGA, "GGA" },
{ GSV, "GSV" },
{ VTG, "VTG" },
{ RMC, "RMC" },
{ GSA, "GSA" },
{ GLL, "GLL" },
{ (NMEATYPE) 0, NULL }, };

const NMEATALKERSTRUCT nmeatalkerstr[] =
{
{ GP, "GP" },
{ GN, "GN" },
{ GL, "GL" },
{ (NMEATALKER) 0, NULL }, };

void
Gps::isr (word address)
{
 if ( Reg->SR & USART_SR_RXNE )
 {
  short ch = Reg->DR;
  Reg->SR &= ~USART_SR_RXNE;
  if ( ready )
  {
   return;
  }

  if ( 0 == nmeastr_len && '$' != ch )
  {
   return;
  }
  else if ( '\n' == ch || NMEA_MAX_LEN <= nmeastr_len + 1 )
  {
   nmeastr[nmeastr_len] = ch;
   ready = true;
   return;
  }
  nmeastr[nmeastr_len++] = ch;
 }
}

NMEATYPE
Gps::get_nmea_sent_type (const char* field)
{
 for (int i = 0; nmeatypesstr[i].str; ++i)
  if ( !strncmp (field + 3, nmeatypesstr[i].str, 3) )
  {
   return nmeatypesstr[i].type;
  }
 DEBUG_LOG (0, "Sender type '%s'\r\n", field);
 return WRONG;
}

NMEATALKER
Gps::get_nmea_talker (const char* field)
{
 DEBUG_LOG (0, "GPS sender: '%s'\r\n", field);
 if ( !strncmp (field + 1, "PMTK", 4) )
 {
  return PMTK;
 }
 for (int i = 0; nmeatalkerstr[i].str; ++i)
 {
  if ( !strncmp (field + 1, nmeatalkerstr[i].str, 2) )
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
 if ( strlen (str) < 7 )
 {
  return;
 }
 //Check for valid string here
 //XXXYY.ZZ or XXYY.ZZ
 char latlon[16] =
 { 0 };
 strcpy (latlon, str);
 char* p = latlon;
 //Find and extract fraction of minutes
 while (*p++ != '.')
  ;

 word minute_fr = str10_to_word (p);
 int secDigits = floor (log10 (abs (minute_fr))) + 1;
 double secPower = pow (10, secDigits);
 c->sec = minute_fr / secPower;
 c->sec *= 60.0;
 *--p = 0;
 //Go on with minutes
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

void
Gps::rttprint ()
{
 if ( !nmea.nmeaok )
 {
  if ( NMEA_ERROR_OK != prepare () )
  {
   return;
  }
 }
 SEGGER_RTT_WriteString (0, nmeastr);
 DEBUG_LOG (0, "Checksum [%X]: %s\n", nmea.checksum,
                    nmea.nmeaok ? "OK" : "ERROR");
 DEBUG_LOG (0, "UTC: %6u\n", nmea.utc);
 DEBUG_LOG (0, "LAT:%3u.%2u\'%2u\" %c\n", nmea.lat.deg, nmea.lat.min,
                    nmea.lat.sec, nmea.lat.dir);
 DEBUG_LOG (0, "LON:%3u.%2u\'%2u\" %c\n", nmea.lon.deg, nmea.lon.min,
                    nmea.lon.sec, nmea.lon.dir);
 reset ();
}

NMEAERR
Gps::prepare (void)
{
 if ( !ready )
 {
  return NMEA_NOT_READY;
 }
 NMEAERR err = NMEA_ERROR_OK;
 //DEBUG_LOG(0, "NMEA: %s\r\n", nmeastr);
 for (int i = 0; i < nmeastr_len; ++i)
 {
  err = parse (nmeastr[i]);
  if ( NMEA_ERROR_OK != err )
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

word
Gps::get_speed ()
{
 word speed = 0;
  if(nmea.knots)
   {
  	speed = ceil(nmea.knots / 1.852);
   }
  if (nmea.kmh)
   {
  	speed = nmea.kmh > speed ? nmea.kmh : speed;
   }
	return speed;
}

int
Gps::get_alt ()
{
	return nmea.alt;
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
 result.deg = c.deg;
 double fract = ((c.sec / 60.0) + c.min) / 60.0;
 double dummy;
 result.fract = (word) round (fabs (modf (fract, &dummy)) * 1e9);
 DEBUG_LOG (0, "%u.%u.%u = %u.%u\r\n", c.deg, c.min, (word) c.sec,
                    result.deg, result.fract);
 return result;
}

bool
Gps::ok ()
{
 return nmea.lat.valid && nmea.lon.valid;
}

bool
Gps::correct_rtc ()
{
 while (NMEA_ERROR_OK != prepare ())
  ;

 if ( !nmea.utc )
 {
  return false;
 }

 Rtc r;
 if ( nmea.utc - r.get () > 5 && nmea.utc < MAX_UNIX_TIMESTAMP )
 {
  r.init (nmea.utc);
  DEBUG_LOG (0, "New RTC value is %u\r\n", r.get ());
 }
 return true;
}
