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

const char GPX_TRACK[]=
"\
<trkpt lat=\"%u.%u\" lon=\"%u.%u\">\
 <time>%6u</time>\
	<fix>%ud</fix>\
  <sat>%ud</sat>\
</trkpt>\
	";

typedef struct
{
 word deg;
 word fract;
}UTM;

UTM coord2utm (double coord)
{
 UTM result;
 result.deg = (word)coord;
 result.fract =(word)(coord * 1000000UL);
 result.fract = result.fract - (result.deg *1000000UL);
 return result;
}

void write_track(Gps g, FATdisk disk, FIL gpx)
{
 unsigned written;
 char text[128];
 FRESULT r;
 for(;;)
	{
	 while(!g.ready);
	 UTM lat = coord2utm(g.get_dec_lat());
	 UTM lon = coord2utm(g.get_dec_lon());
	 xsprintf(text, "");
	 SEGGER_RTT_printf(0, "%s\r\n", text);
	 r = disk.f_write(&gpx, text, strlen(text),&written);
	 if (r != FR_OK)
	 {
		SEGGER_RTT_printf(0, "Failed to write to the file: %s\r\n", disk.result_to_str(r));
		return;
	 }
	 g.reset();
	}
}

int main(void)
{
	Gps g(1, 9600);
	FATdisk d(1);
	FATFS fs;
	FRESULT r;
	r = d.mount(&fs, "0:",1);
	SEGGER_RTT_printf(0, "Disk result: %s\r\n", d.result_to_str(r));
	FIL file;
	r = d.open(&file, "log.gpx", FA_CREATE_ALWAYS | FA_WRITE);
	if (r == FR_OK)
	 {
		write_track(g, d, file);
	 }
	d.close(&file);
	MAIN_END;
}
