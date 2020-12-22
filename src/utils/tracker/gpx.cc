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
#include <common.hpp>
#include <log.hpp>
#include <core/io_macro.hpp>
#include <core/isr_helper.hpp>
#include <core/stm32f10x.hpp>
#include <global.hpp>
#include <xprintf.h>
#include <core/vmmu.hpp>
#include "gpx.hpp"

static const char GPX_HEADER[] =
    "\
<?xml version=\"1.0\"?>\r\n\
<gpx>\r\n\
xmlns=\"http://www.topografix.com/GPX/1/1\"\r\n\
version=\"1.1\"\r\n\
creator=\"Tracker\"\r\n\
  <time>2017</time>\r\n\
    <metadata>\r\n\
      <name>STM32</name>\r\n\
      <desc>Sample track</desc>\r\n\
      <author>\r\n\
        <name>Pugnator</name>\r\n\
      </author>\r\n\
    </metadata>\r\n\
    <trk>\r\n\
      <name>Test run</name>\r\n\
      <trkseg>\r\n";

static const char GPX_TRACK_POINT[] =
    "\
        <trkpt lat=\"%u.%u\" lon=\"%u.%u\">\"\r\n\
          <time>%u</time>\r\n\
          <nmea>%s</nmea>\r\n\
    			<gsv>%u</gsv>\r\n\
        </trkpt>\r\n";

static const char GPX_FOOTER[] =
    "\
      </trkseg>\r\n\
    </trk>\r\n\
</gpx>";

bool
GPX::create (const char* filename)
{
	result = f_mount (&filesystem, "0:", 1);
	PrintF( "Disk result: %s\r\n", result_to_str (result));
	if (FR_OK != result)
	{
	 return false;
	}
	result = f_mkdir("tracks");
	result = f_chdir("tracks");
	if (FR_OK != result)
	{
	  return false;
	}
	result = f_open(&gpx, filename, FA_CREATE_ALWAYS | FA_WRITE);
	if (result != FR_OK)
	{
		PrintF( "Failed to open the file: %s\r\n",
		                   result_to_str (result));
		return false;
	}

	result = f_write (&gpx, GPX_HEADER, strlen (GPX_HEADER), &written);
	return FR_OK == result;
}

bool
GPX::commit ()
{
	result = f_write (&gpx, GPX_FOOTER, strlen (GPX_FOOTER), &written);
	f_close (&gpx);
	return FR_OK == result;
}

bool
GPX::set_point (void)
{
	while (NMEA_ERROR_OK != gps->prepare ())
		;
	if (!gps->ok())
	{
		gps->reset ();
		return false;
	}

	coord lat = gps->getlat ();
	coord lon = gps->getlon ();
	UTM latutm = gps->coord2utm (lat);
	UTM lonutm = gps->coord2utm (lon);
	char *text = (char*)stalloc(256);
	xsprintf (text, GPX_TRACK_POINT, latutm.deg, latutm.fract, lonutm.deg,
	          lonutm.fract, gps->get_utc(), gps->nmeastr, gps->gsv);

	PrintF( "GPS: %s\r\n", text);

	result = f_write (&gpx, text, strlen (text), &written);
	stfree(text);
	if (result != FR_OK)
	{
		PrintF( "Failed to write to the file: %s\r\n",
		                   result_to_str (result));
		return false;
	}

	result = f_sync(&gpx);

	gps->reset ();
	return FR_OK == result;
}
