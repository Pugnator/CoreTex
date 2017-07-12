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
 *  GSA - GPS DOP and active satellites.
 *  This sentence provides details on the nature of the fix. It includes the numbers of the satellites being used in
 *  the current solution and the DOP. DOP (dilution of precision) is an indication of the effect of satellite geometry
 *  on the accuracy of the fix. It is a unitless number where smaller is better. For 3D fixes using 4 satellites a 1.0
 *  would be considered to be a perfect number, however for overdetermined solutions it is possible to see numbers below 1.0.

There are differences in the way the PRN's are presented which can effect the ability of some programs to display this data.
For example, in the example shown below there are 5 satellites in the solution and the null fields are scattered indicating
that the almanac would show satellites in the null positions that are not being used as part of this solution.
Other receivers might output all of the satellites used at the beginning of the sentence with the null field all
stacked up at the end. This difference accounts for some satellite display programs not always being able to display the satellites
being tracked. Some units may show all satellites that have ephemeris data without regard to their use as part of the solution but
this is non-standard.

  $GPGSA,A,3,04,05,,09,12,,,24,,,,,2.5,1.3,2.1*39

Where:
     GSA      Satellite status
     A        Auto selection of 2D or 3D fix (M = manual)
     3        3D fix - values include: 1 = no fix
                                       2 = 2D fix
                                       3 = 3D fix
     04,05... PRNs of satellites used for fix (space for 12)
     2.5      PDOP (dilution of precision)
     1.3      Horizontal dilution of precision (HDOP)
     2.1      Vertical dilution of precision (VDOP)
     *39      the checksum data, always begins with *
 *
 */

typedef enum NMEAFORMAT
{
 TYPE , LAT, LATD, LON, LOND, FIXUTC, CS, END
} NMEAFORMAT;

void Gps::fillGLLctx(int sect, const char* field)
{
	switch (sect)
	{
		default:
			break;
	}
}
