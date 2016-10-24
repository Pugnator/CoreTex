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
#include <tests/tests.hpp>
#include <core/spi.hpp>
#include <drivers/console.hpp>
#include <drivers/nrf24.hpp>
#include <drivers/storage/disk.hpp>
class Console *__dbg_out;

int main(void)
  {
    Uart u(1, CONSOLE_SPEED);
    Console out(&u);
    out.cls();
    __dbg_out = &out;
    LOGPRINT("Core started\r\n");
    Spi e(1);
    e.read(0x8);
    uint8_t resp = e.read();
    LOGPRINT("SETUP_RETR = 0x%x\r\n", resp);
	//disk_test();
    MAIN_END;
   //dbgout.xprintf("System started\n");
   //GPS::Gps g(2, 115200);
   //MODEM::Modem mdm(3, 19200, &dbgout);
   //dbgout.xprintf("SIM900 %s\n", mdm.ok ? "ON" : "OFF");
   //dbgout.xprintf("SIM900 setup: %s\n", mdm.setup() ? "OK" : "FAIL");
   //dbgout.xprintf("SMS number: %lu\n", mdm.get_sms_amount());
  }
