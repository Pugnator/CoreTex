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


int main(void)
{
 SEGGER_RTT_printf(LOGGING_CHANNEL, "CPU started\r\n");
 bc417 b(1, 9600);
 TRY
 {
  b.test();
  b.set_name("TEST");
  b.set_pin("1234");
 }
 CATCH(ERROR_GENERIC)
 {
  SEGGER_RTT_printf(LOGGING_CHANNEL, "Error occured\r\n");
 }
 ETRY;
 MAIN_END;
}
