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
#include <core/spi.hpp>
#include <drivers/console.hpp>
#include <drivers/ov528.hpp>
#include <drivers/storage/fatdisk.hpp>
#include <drivers/storage/ff.hpp>

int main(void)
{
	Uart u(1, CONSOLE_SPEED);
	Console out(&u);
	out.cls();
	out.print("!!!");
	RTTPRINT("\r\nCore started\r\n");
	RTTPRINT("FatFs test started\r\n");
	DISK::FATdisk a(1);
	DISK::FRESULT r;
	DISK::FATFS fs;
	r = a.mount(&fs, "0:", 1);
	SEGGER_RTT_printf(0,"f_mount %s\r\n", a.result_to_str(r));
	DISK::FIL test;
	r = a.open(&test, "test.txt", FA_OPEN_ALWAYS | FA_WRITE |FA_READ);
	SEGGER_RTT_printf(0,"open %s\r\n", a.result_to_str(r));
	UINT bw = 0;
	const char* text= "Hellow world!";
	r = a.f_write(&test, text, strlen(text), &bw);
	SEGGER_RTT_printf(0,"f_write %s\r\n", a.result_to_str(r));
	r = a.close(&test);
	SEGGER_RTT_printf(0,"close %s\r\n", a.result_to_str(r));
	//LOGPRINT("Status: 0x%x\r\n", a.get_status());
	//r = f_mkfs("0:", 0, 0);
	//LOGPRINT("f_mount? %s\r\n", a.FRESULT2str(r));
	//FIL test;
	//r = f_open(&test, "test.txt", FA_OPEN_ALWAYS);
	//LOGPRINT("f_open? %s\r\n", get_fresult(r));
	//f_close(&test);
	//LOGPRINT("f_close? %s\r\n", get_fresult(r));
	MAIN_END;
	//dbgout.xprintf("System started\n");
	//GPS::Gps g(2, 115200);
	//MODEM::Modem mdm(3, 19200, &dbgout);
	//dbgout.xprintf("SIM900 %s\n", mdm.ok ? "ON" : "OFF");
	//dbgout.xprintf("SIM900 setup: %s\n", mdm.setup() ? "OK" : "FAIL");
	//dbgout.xprintf("SMS number: %lu\n", mdm.get_sms_amount());
}
