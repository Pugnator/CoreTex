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
#include <utils/tracker/gpx.hpp>
#include <drivers/gps.hpp>
#include <drivers/storage/fatdisk.hpp>

int main (void)
{
 FATdisk d(1);
 FATFS filesystem;
 FIL gpx;
 FRESULT result = d.mount (&filesystem, "0:", 1);
 SEGGER_RTT_printf (0, "Disk result: %s\r\n", d.result_to_str (result));

 result = d.open (&gpx, "test.txt", FA_CREATE_ALWAYS | FA_WRITE);
 if (result != FR_OK)
 {
   SEGGER_RTT_printf (0, "Failed to open the file: %s\r\n",  d.result_to_str (result));
 }
 d.close(&gpx);
 MAIN_END;





 SEGGER_RTT_WriteString (0, "CPU started\r\n");
 Gps g (1, 9600);
 GPX tracker(&g);
 tracker.create("log.txt");
 word count = 0;
 for(;;)
 {
	 if(tracker.do_point())
	 {
		 BLINK;
	 }

 }
 tracker.commit();
 PIN_HI(LED);
 MAIN_END;
}
