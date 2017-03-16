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

/*
 $GPGLL

Geographic Position, Latitude / Longitude and time.

eg1. $GPGLL,3751.65,S,14507.36,E*77
eg2. $GPGLL,4916.45,N,12311.12,W,225444,A


           4916.46,N    Latitude 49 deg. 16.45 min. North
           12311.12,W   Longitude 123 deg. 11.12 min. West
           225444       Fix taken at 22:54:44 UTC
           A            Data valid


eg3. $GPGLL,5133.81,N,00042.25,W*75
               1    2     3    4 5

      1    5133.81   Current latitude
      2    N         North/South
      3    00042.25  Current longitude
      4    W         East/West
      5    *75       checksum

$--GLL,lll.ll,a,yyyyy.yy,a,hhmmss.ss,A llll.ll = Latitude of position

a = N or S
yyyyy.yy = Longitude of position
a = E or W
hhmmss.ss = UTC of position
A = status: A = valid data
 */

typedef enum NMEAFORMAT
{
 TYPE , LAT, LATD, LON, LOND, FIXUTC, CS, END
} NMEAFORMAT;

void Gps::fillGLLctx(int sect, const char* field)
{

}
