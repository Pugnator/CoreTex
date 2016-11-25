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
#include <core/usart.hpp>
#include <drivers/gsm.hpp>

int main(void)
{
	SEGGER_RTT_printf(0, "CPU started\r\n");
	Modem m(1, 9600);
	if(!m.setup())
	{
	 SEGGER_RTT_printf(0, "BAD\r\n");
	 MAIN_END;
	}
	SEGGER_RTT_printf(0, "OK\r\n");
	SEGGER_RTT_printf(0, "Debet: %u\r\n", m.get_account_debet(BEELINE));
	MAIN_END;
}
