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
#include <drivers/storage/mmc/fat32/ff.h>
#include <hal/adc.hpp>
#include <hal/spi.hpp>
#include <hal/usart.hpp>
#include <hal/vmmu.hpp>
#include <drivers/gps.hpp>
#include <drivers/gsm.hpp>
#include <drivers/mmc.hpp>
#include <drivers/ov528.hpp>
#include <drivers/console.hpp>
#include <drivers/nrf24.hpp>
#include <drivers/bc470.hpp>
#include <drivers/xmodem.hpp>

using namespace HAL;
using namespace NRF24;
using namespace ADC;
using namespace SPI;
using namespace OV528;
using namespace MMC;
using namespace XMODEM;
using namespace BC470;

DSTATUS disk_initialize (BYTE drv = 0)
  {
    return 0;
  }
DSTATUS disk_status (BYTE drv = 0)
  {
    return 0;
  }
DRESULT disk_read (BYTE drv,BYTE* buff,DWORD sector,UINT count)
  {
    return RES_NOTRDY;
  }
DRESULT disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
  {
    return RES_NOTRDY;
  }
DWORD get_fattime (void)
  {
    return RES_NOTRDY;
  }
DRESULT disk_ioctl (BYTE drv,BYTE cmd,void* buff)
  {
    return RES_NOTRDY;
  }

void inline infinite_loop(void)
  {
    for (;;)
      ;
  }

int main(void)
  {
    Console dbgout(Uart(1, CONSOLE_SPEED));
    dbgout.cls();
    dbg_print("Hello world\n");
    dbgout.print("!!!");
    //dbgout.xprintf("System started\n");
    //GPS::Gps g(2, 115200);
    //MODEM::Modem mdm(3, 19200, &dbgout);
    //dbgout.xprintf("SIM900 %s\n", mdm.ok ? "ON" : "OFF");
    //dbgout.xprintf("SIM900 setup: %s\n", mdm.setup() ? "OK" : "FAIL");
    //dbgout.xprintf("SMS number: %lu\n", mdm.get_sms_amount());

    infinite_loop();
  }
