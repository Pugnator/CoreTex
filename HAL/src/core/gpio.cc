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

#include <core/gpio.hpp>

namespace IO
{

 GPIO_pin::GPIO_pin (PINCFG conf)
 {
	switch (conf.speed)
	{
	 case IOSPEED_10MHz:
		break;
	 case IOSPEED_20MHz:
		break;
	 case IOSPEED_50MHz:
		break;
	 default:
		break;
	}

	switch (conf.mode)
	{
	 case IN_PD:
		break;
	 case IN_PU:
		break;
	 case IN_FLT:
		break;
	 case IN_ANALOG:
		break;
	 case OUT_PP:
		break;
	 case OUT_OD:
		break;
	 case OUT_ALT_PP:
		break;
	 case OUT_ALT_OD:
		break;
	 default:
		break;
	}
 }

 GPIO_pin::~GPIO_pin ()
 {
 }

 GPIO::GPIO (IOPORT _port)
 {
	port = _port;
 }

 GPIO::~GPIO ()
 {
 }
}
