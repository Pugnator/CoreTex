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
#include <drivers/gsm.hpp>
#include <drivers/storage/fatdisk.hpp>
#include <drivers/storage/eeprom.hpp>
#include <core/rtc.hpp>
#include <core/usart.hpp>
#include <drivers/ov528.hpp>
#include <core/iic.hpp>

void track()
{
	Gps g(1, 9600);
	 	GPX tr(&g);
	 	tr.create("log.txt");
	 	for(;;)
	 	{
	 		if(tr.set_point())
	 		{
	 			BLINK;
	 		}
	 		SEGGER_RTT_printf(0, "UTC: %u\r\n", g.get_utc());
	 		g.correct_rtc();
	 	}
	tr.commit();
}

void gsm()
{
	GSM m(2, 38400);
		if (m.setup())
		{
		 SEGGER_RTT_WriteString(0, "GSM inited\r\n");
		}
		else
		{
		 MAIN_END;
		}

		for(;;)
		{
		 m.get_cc_info();
		 delay_ms(5000);
		}

}

int main (void)
{
	SEGGER_RTT_WriteString(0, "Started\r\n");
	Eeprom e;
	e.cell_write(2, 0xAA);

	MAIN_END;
}
