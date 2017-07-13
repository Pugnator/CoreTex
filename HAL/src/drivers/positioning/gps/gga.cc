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

/*
 * GGA - essential fix data which provide 3D location and accuracy data.

 $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47

Where:
     GGA          Global Positioning System Fix Data
     123519       Fix taken at 12:35:19 UTC
     4807.038,N   Latitude 48 deg 07.038' N
     01131.000,E  Longitude 11 deg 31.000' E
     1            Fix quality: 0 = invalid
                               1 = GPS fix (SPS)
                               2 = DGPS fix
                               3 = PPS fix
			       4 = Real Time Kinematic
			       5 = Float RTK
                               6 = estimated (dead reckoning) (2.3 feature)
			       7 = Manual input mode
			       8 = Simulation mode
     08           Number of satellites being tracked
     0.9          Horizontal dilution of position
     545.4,M      Altitude, Meters, above mean sea level
     46.9,M       Height of geoid (mean sea level) above WGS84
                      ellipsoid
     (empty field) time in seconds since last DGPS update
     (empty field) DGPS station ID number
 *47          the checksum data, always begins with *
 *
 */

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
 ALT,
 ALTU,
 GEOSEP,
 GEOSEPU,
 AGE,
 ID,
 CS,
 END
} NMEAFORMAT;

void Gps::fillGGActx(int sect, const char* field)
{
 if(!*field)
 {
	return;
 }
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
	case ALT:
	 *strchr(field, '.') = 0;
	 nmea.alt = str10_to_word(field);
	 break;
	case END:
	 break;
 }
}
