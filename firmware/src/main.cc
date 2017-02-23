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
#include <global.hpp>
#include <common.hpp>
#include <log.hpp>
#include <xprintf.h>
#include <xml/xml.hpp>
#include <drivers/storage/fatdisk.hpp>
#include <drivers/gps.hpp>

const char GPX_HEADER[] =
  "\
<?xml version=\"1.0\"?>\r\n\
<gpx>\r\n\
xmlns=\"http://www.topografix.com/GPX/1/1\"\r\n\
version=\"1.1\"\r\n\
creator=\"Tracker\"\r\n\
  <time>2017</time>\r\n\
	  <metadata>\r\n\
	    <name>STM32</name>\r\n\
	    <desc>Description</desc>\r\n\
	    <author>\r\n\
	      <name>Autor</name>\r\n\
	    </author>\r\n\
	  </metadata>\r\n\
	  <trk>\r\n\
	    <name>Test run</name>\r\n\
	    <trkseg>\r\n";

const char GPX_TRACK_POINT[] =
  "\
	      <trkpt lat=\"%3u.%2u\'%2u\" %c\" lon=\"%3u.%2u\'%2u\" %c\">\"\r\n\
	        <time>%u</time>\r\n\
	      </trkpt>\r\n";

const char GPX_FOOTER[] = "\
	    </trkseg>\r\n\
    </trk>\r\n\
</gpx>";

typedef struct
{
 word deg;
 word fract;
} UTM;

UTM
coord2utm (double coord)
{
 UTM result;
 result.deg = (word) coord;
 result.fract = (word) (coord * 10000UL);
 result.fract = result.fract - (result.deg * 10000UL);
 return result;
}

int
main (void)
{
	SEGGER_RTT_WriteString (0, "CPU started\r\n");
 Gps g (1, 9600);
 for(;;)
	{
	 g.rttprint();
	 g.reset();
	}
 SEGGER_RTT_printf (0, "%s", RTT_CTRL_RESET);
 FATdisk disk (1);
 FATFS fs;
 FRESULT r;
 r = disk.mount (&fs, "0:", 1);
 SEGGER_RTT_printf (0, "Disk result: %s\r\n", disk.result_to_str (r));

 unsigned written;
 char text[128];
 FIL gpx;
 r = disk.open (&gpx, "log.gpx", FA_CREATE_ALWAYS | FA_WRITE);
 if (r != FR_OK)
	{
	 SEGGER_RTT_printf (0, "Failed to open the file: %s\r\n",
		                  disk.result_to_str (r));
	 MAIN_END
	 ;
	}
 r = disk.f_write (&gpx, GPX_HEADER, strlen (GPX_HEADER), &written);
 for (word count = 0; count < 10; count++)
	{
	 BLINK;
	 while (NMEA_ERROR_OK != g.prepare ())
		;
	 coord lat = g.getlat ();
	 coord lon = g.getlon ();
	 xsprintf (text, GPX_TRACK_POINT, lat.deg, lat.min, lat.sec, lat.dir, lon.deg,
		         lon.min, lon.sec, lon.dir, g.get_utc ());
	 SEGGER_RTT_printf (0, "GPS: %s\r\n", text);
	 g.rttprint ();
	 r = disk.f_write (&gpx, text, strlen (text), &written);
	 if (r != FR_OK)
		{
		 SEGGER_RTT_printf (0, "Failed to write to the file: %s\r\n",
			                  disk.result_to_str (r));
		 MAIN_END
		 ;
		}
	 g.reset ();
	 if (count % 10)
		{
		 SEGGER_RTT_WriteString (0, "flushing data to the disk\r\n");
		 disk.flush (&gpx);
		}
	}
 r = disk.f_write (&gpx, GPX_FOOTER, strlen (GPX_FOOTER), &written);
 disk.close (&gpx);
 MAIN_END
 ;
}
