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

/*
 $GPGSV

 GPS Satellites in view

 eg. $GPGSV,3,1,11,03,03,111,00,04,15,270,00,06,01,010,00,13,06,292,00*74
 $GPGSV,3,2,11,14,25,170,00,16,57,208,39,18,67,296,40,19,40,246,00*74
 $GPGSV,3,3,11,22,42,067,42,24,14,311,43,27,05,244,00,,,,*4D


 $GPGSV,1,1,13,02,02,213,,03,-3,000,,11,00,121,,14,13,172,05*67


 1    = Total number of messages of this type in this cycle
 2    = Message number
 3    = Total number of SVs in view
 4    = SV PRN number
 5    = Elevation in degrees, 90 maximum
 6    = Azimuth, degrees from true north, 000 to 359
 7    = SNR, 00-99 dB (null when not tracking)
 8-11 = Information about second SV, same as field 4-7
 12-15= Information about third SV, same as field 4-7
 16-19= Information about fourth SV, same as field 4-7
 */
namespace GPS
{
typedef enum NMEAFORMAT
{
 TYPE,
 MSGTYPNUM,
 MSGNUM,
 SATNUM,
 SVPRN1,
 ELEV1,
 AZ1,
 SNR1,
 SVPRN2,
 ELEV2,
 AZ2,
 SNR2,
 SVPRN3,
 ELEV3,
 AZ3,
 SNR3,
 SVPRN4,
 ELEV4,
 AZ4,
 SNR4,
 CS,
 END
} NMEAFORMAT;

void
Gps::fillGSVctx (int sect, const char* field)
{
 switch (sect)
 {
  case SATNUM:
   gsv = str10_to_word (field);
   break;
  default:
   break;
 }
}
}
