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
#include <core/isr_helper.hpp>
#include <core/vmmu.hpp>
#include <drivers/gsm.hpp>
#include <stdarg.h>
#include <log.hpp>

namespace MODEM
{
 class Modem *Modem::self = nullptr;

 void Modem::modemisr(void)
 {
  volatile uint16_t __SR = self->Reg->SR;
  if (__SR & USART_SR_RXNE)
  {
   __SR &= ~USART_SR_RXNE;
   volatile uint16_t a = self->Reg->DR;
   SEGGER_RTT_printf(0, "%c", a);
   if (!self->go || self->buflen >= MODEM_IN_BUFFER_SIZE)
   {
    self->go = false;
    return;
   }
   self->modembuf[self->buflen++] = a;
  }
  else if (__SR & USART_SR_TC)
  {
   __SR &= ~USART_SR_TC;
  }
  self->Reg->SR = __SR;
 }

 void Modem::reset(void)
 {
  buflen = 0;
  memset(modembuf, 0, sizeof modembuf);
  go = true;
 }

 void Modem::sync_speed(void)
 {
  char try_count = 15;
  do
  {
   --try_count;
   reset();
   rawcmd(CMD::AT, CMDMODE::EXEC, nullptr);
  }
  while (try_count && !wait_for_reply(CMD::AT, AT_OK, 200));
 }

 bool Modem::setup(void)
 {
	 if (!set(CMD::CMEE, "2"))
	   {
	    return false;
	   }

  if (!set(CMD::CMGF, "1"))
  {
   return false;
  }

  if (!set(CMD::CNMI, "0,0,0,0,0"))
  {
   return false;
  }

  if (!set(CMD::CSCS, "\"UCS2\""))
  {
   return false;
  }
  if (!set(CMD::CSMP, "17,167,0,8"))
  {
   return false;
  }

  if (!set(CMD::CPMS, "\"SM\""))
  {
   return false;
  }
  return true;
 }

 bool Modem::send_sms(const char* number, const char* text)
 {
	 char num[128];
	 ascii2ucs2(number, num, 128);
  rawcmd(CMD::CMGS, CMDMODE::SET, num);

  if (!wait_for_reply(CMD::CMGS, AT_INPUT_PROMPT))
  {
   SEGGER_RTT_printf(0, "No input prompt\r\n");
   return false;
  }
  writestr(text);
  write(0x1A);
  return true;
 }

 char* Modem::get_sms_by_index(word num)
 {
  rawcmd(CMD::CMGR, CMDMODE::SET, num);
  if (!wait_for_reply(CMD::CPMS, AT_OK))
  {
   return nullptr;
  }
  go = false;
  char *sms = extract_sms_body(strclone(modembuf));
  reset();
  return sms;
 }

 char* Modem::extract_sms_body(char *message)
 {
  const char delimeter[] = "\r\n";
  char *token = strtok(message, delimeter);
  while (token)
  {
   //TODO: extract telnum, date, etc
   if (!strstr(token, "+CMGR") && *token != 'O')
   {
    //here we've got sms encoded in UCS2
    return ucs2ascii(token);
   }
   token = strtok(NULL, delimeter);
  }
  FREE(message);
  return nullptr;
 }

 bool Modem::delete_all_sms(void)
 {
  rawcmd(CMD::CMGD, CMDMODE::SET, "1,4");
  if (!wait_for_reply(CMD::CMGD, AT_OK))
  {
   return false;
  }
  return true;
 }

 word Modem::get_sms_amount(void)
 {
  for (word i = 1; i < 50; ++i)
  {
   char *sms = get_sms_by_index(i);
   if (!sms)
   {
    return i - 1;
   }
   FREE(sms);
  }
  return 0;
 }

 void Modem::restart(void)
 {
  rawcmd(CMD::CFUN, CMDMODE::SET, "1,1");
 }

 void Modem::turn_off(void)
 {
  rawcmd(CMD::CPOWD, CMDMODE::SET, 1);
 }

}
