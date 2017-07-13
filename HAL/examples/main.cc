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
#include <drivers/storage/fatdisk.hpp>

int
main (void)
{
 FATdisk d (1);
 FATFS fs;
 FRESULT r;
 r = d.mount (&fs, "0:", 1);
 SEGGER_RTT_printf (0, "Disk result: %s\r\n", d.result_to_str (r));
 FIL file;
 /*r = d.open(&file, "write.txt", FA_CREATE_ALWAYS | FA_WRITE);
  if (r == FR_OK)
  {
  d.close(&file);
  }
  else
  {
  SEGGER_RTT_printf(0, "Write result: %s\r\n", d.result_to_str(r));
  }*/

 MAIN_END
 ;
}
