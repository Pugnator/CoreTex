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
#include <drivers/storage/fat32/ff.h>
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

void inline infinite_loop(void)
  {
    for (;;)
      ;
  }

int main(void)
  {
	infinite_loop();
    Uart test(1, CONSOLE_SPEED);
    test.writestr("HI THERE!");
    //dbg_print("Hello world\n");
    //dbgout.print("!!!");
    //dbgout.xprintf("System started\n");
    //GPS::Gps g(2, 115200);
    //MODEM::Modem mdm(3, 19200, &dbgout);
    //dbgout.xprintf("SIM900 %s\n", mdm.ok ? "ON" : "OFF");
    //dbgout.xprintf("SIM900 setup: %s\n", mdm.setup() ? "OK" : "FAIL");
    //dbgout.xprintf("SMS number: %lu\n", mdm.get_sms_amount());


  }
