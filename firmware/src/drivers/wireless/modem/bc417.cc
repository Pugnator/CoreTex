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

#include "drivers/bc417.hpp"
#include <global.hpp>


bool bc417::test()
{
 ok = false;
 rawcmd(CMD::AT, CMDMODE::RAW);
 if(!wait_for_reply(CMD::AT, AT_OK, REPLY_TIMEOUT))
 {
  return ok;
 }
 return ok = true;
}

void bc417::set_name(const char *name)
{
 ok = false;
 rawcmd(CMD::NAME, CMDMODE::RAW, name);
 if(!wait_for_reply(CMD::AT, AT_OK, REPLY_TIMEOUT))
 {
  return;
 }
 ok = true;
}

void bc417::set_pin(const char *pin)
{
 ok = false;
 rawcmd(CMD::PIN, CMDMODE::RAW, pin);
 if(!wait_for_reply(CMD::AT, AT_OK, REPLY_TIMEOUT))
 {
  return;
 }
 ok = true;
}
