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

#include <drivers/storage/fatdisk.hpp>
#include <log.hpp>
#include <common.hpp>

namespace LOG
{

SDcardLog::SDcardLog()
{
  open();
}

bool SDcardLog::open()
{
  logdisk.reset(new FATdisk(CHANNEL_1));
  do
  {
    fresult = logdisk->f_mount(&fs, "0:", 1);
    delay_ms(500);
  } while (fresult != FR_OK);

  fresult = logdisk->f_open(&logf, "log.txt", FA_OPEN_APPEND | FA_WRITE);
  if (FR_OK != fresult)
  {
    PrintF("Error: %s\r\n", logdisk->result_to_str(fresult));
    return false;
  }

  UINT bw = 0;
  fresult = logdisk->f_write(&logf, "Started\r\n", 9, &bw);
  if (FR_OK != fresult)
  {
    PrintF("Error: %s\r\n", logdisk->result_to_str(fresult));
    return false;
  }

  PrintF("Logging started\r\n");
  return true;
}

void SDcardLog::close()
{
  logdisk->f_sync(&logf);
  logdisk->f_close(&logf);
  logdisk->f_mount(nullptr, "0:", 1);
}

void SDcardLog::write(const char *text)
{
  UINT bw = 0;
  fresult = logdisk->f_write(&logf, text, strlen(text), &bw);
  if (fresult != FR_OK)
    PrintF("Error: %s\r\n", logdisk->result_to_str(fresult));
}

} // namespace LOG