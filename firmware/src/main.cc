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
#include <drivers/bc417.hpp>
#include <errors.hpp>

int main (void)
{
	SEGGER_RTT_WriteString(0, "Started\n");
	bc417 b(1, 9600);
	b.set_name("BT417");
	b.set_pin("1111");
	for(;;)
	{
		TRY
		{
			b.is_connected();
		}
		CATCH (ERROR_BC471_COMMTEST_FAILED)
		{
			SEGGER_RTT_WriteString(0, "HC-05 is connected\n");
		}
		CATCH (ERROR_BC471_COMMTEST_OK)
		{
			SEGGER_RTT_WriteString(0, "HC-05 is in command mode\r\n");
		}
		ETRY;
		delay_ms(1000);
	}
 MAIN_END;
}
