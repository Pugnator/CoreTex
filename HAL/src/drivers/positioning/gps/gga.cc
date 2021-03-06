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
 * 2015
 *******************************************************************************/
#include <drivers/gps.hpp>
#include <common.hpp>

typedef enum NMEAFORMAT
{
  TYPE,
  UTC,
  LAT,
  LATDIR,
  LONG,
  LONGDIR,
  QUAL,
  SATNUM,
  HDOP,
  MSL,
  ALT,
  GEOSEP,
  GEOSEPU,
  AGE,
  ID,
  CS,
  END
} NMEAFORMAT;

void Gps::fillGGActx(int sect, const char *field)
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
  case MSL:
    nmea.msl = str10_to_word(field);
    break;
  case QUAL:
    fix = str10_to_word(field) > 0 ? true : false;
    break;
  case END:
    break;
  }
}
