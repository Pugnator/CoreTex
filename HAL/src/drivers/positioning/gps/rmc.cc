/*******************************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 2015, 2016
 *******************************************************************************/
#include <drivers/gps.hpp>
#include <common.hpp>
#include <log.hpp>
#include <math.h>

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
  TYPE,
  UTC,
  STATUS,
  LAT,
  LATDIR,
  LONG,
  LONGDIR,
  KNOTS,
  TMG,
  DATE,
  CS,
  LONGDIR2,
  END
} NMEAFORMAT;

void Gps::fillRMCctx(int sect, const char *field)
{
  switch (sect)
  {
  case UTC:
    *strchr(field, '.') = 0;
    nmea.utc = str10_to_word(field);
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
  case KNOTS:
    nmea.kmh = round(1.852 * strtod(field, NULL));
    PrintF("Speed = %s * 1.852 = %u\r\n", field, nmea.kmh);
    break;
  case END:
    break;
  }
}
