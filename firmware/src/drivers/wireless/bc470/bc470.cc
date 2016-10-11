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
#include <drivers/bc470.hpp>
#include <hal/vmmu.hpp>
#include <string.h>

namespace HAL
{
namespace BC470
{

const char *BC470CMD[] =
  {
  "AT\r"
  };

const char *RESPONSE_TEXT[] =
  {
  "OK", "ERROR",
  };

class bc470 *bc470::self = nullptr;

bool bc470::factory_default(void)
  {
    rawcmd(AT, nullptr);
    if (!wait4reply(500))
      {
        return false;
      }
    return true;
  }

void bc470::rawcmd(ATCMD cmd, const char* arg)
  {
    reset();
    writestr(BC470CMD[cmd]);
  }

void bc470::reset(void)
  {
    buflen = 0;
    memset(modembuf, 0, sizeof modembuf);
    go = true;
  }

bool bc470::wait4reply(word timeout)
  {
    ok = false;
    char *buf = (char*) ALLOC(sizeof modembuf);
    if (!buf) return false;
    WAIT_FOR(timeout);
    do
      {
        memcpy(buf, modembuf, sizeof modembuf);
        //explicit check for error
        if (strstr(buf, RESPONSE_TEXT[AT_ERROR]))
          {
            FREE(buf);
            return !(ok = false);
          }
        //do we hit the response we expect?
        else if (strstr(buf, RESPONSE_TEXT[AT_OK]))
          {
            return ok = true;
          }
      }
    while (STILL_WAIT);
    FREE(buf);
    conout->xprintf("%s\n", modembuf);
    return ok;
  }

void bc470::bc470isr(void)
  {
    BLINK;
    if (self->Reg->SR & USART_SR_RXNE)
      {
        uint16_t tmp = self->Reg->DR;
        self->Reg->SR &= ~USART_SR_RXNE;
        if (!self->go || self->buflen >= MODEM_IN_BUFFER_SIZE)
          {
            self->go = false;
            return;
          }
        self->modembuf[self->buflen++] = tmp;
      }
    else if (self->Reg->SR & USART_SR_TC)
      {
        self->Reg->SR &= ~USART_SR_TC;
      }
  }

}
}
