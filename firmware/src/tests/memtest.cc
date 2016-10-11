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

#include <global.hpp>
#include <common.hpp>
#include <stdlib.h>
#include <hal/vmmu.hpp>
#include <drivers/console.hpp>

bool memtest(word times)
{
 bool ok = false;
 CONSOLE::Console out(1, CONSOLE_SPEED);
 out < "Memory allocation test";
 for (word i = 0; i < times; ++i)
 {
  void *tmp = ALLOC(900);
  FREE(tmp);
 }
 print_memstat(&out);
 return ok;
}
