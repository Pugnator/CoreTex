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
#include <stdarg.h>
#include <log.hpp>
#include "drivers/gsm.hpp"

class GSM *GSM::self = nullptr;

#define USSD_TIMEOUT 10000
#define USSD_REPLY_TIMEOUT 500

void GSM::sync_speed(void)
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

char* GSM::ussd(const char *request)
{
 ok = false;
 rawcmd(CMD::CUSD, CMDMODE::SET, "1,\"#102#\"");
 if (!wait_for_reply(CMD::CUSD, AT_OK))
 {
  go = false;
  return nullptr;
 }
 SEGGER_RTT_printf(0, "Waiting for USSD reply [10secs]\r\n");
 WAIT_FOR(USSD_TIMEOUT);
 while(STILL_WAIT)
 {
  if(strstr(modembuf, "+CUSD:"))
  {
   delay_ms(USSD_REPLY_TIMEOUT);
   break;
  }
 }

 SEGGER_RTT_printf(0, "Got USSD reply\r\n");
 go = false;
 char *buf = strclone(modembuf);
 if(!buf)
 {
  return nullptr;
 }
 return buf;
}

bool GSM::setup(void)
{
 if (!set(CMD::CMEE, "2"))
 {
  return false;
 }

 if (!set(CMD::CMGF, "1"))
 {
  return false;
 }

 if (!set(CMD::CLIP, "1"))
 {
  return false;
 }

 if (!set(CMD::CREG, "1"))
 {
  return false;
 }

 if (!set(CMD::CNMI, "0,0,0,0,0"))
 {
  return false;
 }

 if (!set(CMD::CSCS, "\"GSM\""))
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

bool GSM::send_sms(const char* number, const char* text)
{
 rawcmd(CMD::CMGS, CMDMODE::SET, number);

 if (!wait_for_reply(CMD::CMGS, AT_INPUT_PROMPT))
 {
  SEGGER_RTT_printf(0, "No input prompt\r\n");
  return false;
 }
 writestr(text);
 write(0x1A);
 return true;
}

char* GSM::get_sms_by_index(word num)
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

word GSM::get_account_debet(CELLULAR_OP op)
{
 ok = false;
 char *buf = nullptr;
 switch (op)
 {
  case BEELINE:
   buf = ussd("\"#102#\"");
   break;
  case MTS:
   buf = ussd("\"#102#\"");
   break;
  case MEGAFON:
   buf = ussd("\"#102#\"");
   break;
  case TELE2:
   buf = ussd("\"#102#\"");
   break;
  default:
   return 0;
 }
 char *bal = strstr(buf, " r.");
 word result = 0;
 if(!bal)
 {
  return result;
 }
 *bal = 0;
 while(*(--bal) != ' ' && bal != buf);
 bal++;
 char *dot = strstr(bal, ".");
 if(dot)
 {
  *dot = result;
 }
 SEGGER_RTT_printf(0, "Balance: %s\r\n", bal);
 ok = true;

 result = str10_to_word(bal);
 FREE(buf);
 return result;
}

char* GSM::extract_sms_body(char *message)
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

bool GSM::delete_all_sms(void)
{
 rawcmd(CMD::CMGD, CMDMODE::SET, "1,4");
 if (!wait_for_reply(CMD::CMGD, AT_OK))
 {
  return false;
 }
 return true;
}

word GSM::get_sms_amount(void)
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

void GSM::restart(void)
{
 rawcmd(CMD::CFUN, CMDMODE::SET, "1,1");
}

void GSM::turn_off(void)
{
 rawcmd(CMD::CPOWD, CMDMODE::SET, 1);
}

bool GSM::get(CMD::ATCMD cmd)
{
 return wait_for_reply(cmd, AT_OK);
}

bool GSM::set(CMD::ATCMD cmd, const char* value)
{
 rawcmd(cmd, CMDMODE::SET, value);
 return wait_for_reply(cmd, AT_OK);
}

bool GSM::is_supported(CMD::ATCMD cmd)
{
 rawcmd(cmd, CMDMODE::CHECK, nullptr);
 return wait_for_reply(cmd, AT_OK);
}
