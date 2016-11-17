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
#include <core/core.hpp>

int main(void)
{
 SEGGER_RTT_printf(0, "CPU started\r\n");
 UART::Uart u(1, 9600);
 u.dma_on();
 //u.dmarx_go(2);
 u.dmatx_go(3);
 delay_ms(500);
 SEGGER_RTT_printf(0, "Result: 0x%X 0x%X 0x%X\r\n", *u.get_rx_buf(), u.get_rx_buf()[1], u.get_rx_buf()[2]);
 MAIN_END;
}
