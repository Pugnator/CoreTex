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
#include <xprintf.h>
#include <log.hpp>
#include "drivers/gsm.hpp"

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
  } while (try_count && !wait_for_reply(CMD::AT, AT_OK, 200));
}

char *GSM::ussd(const char *request)
{
  ok = false;
  rawcmd(CMD::CUSD, CMDMODE::WRITE, "1,\"#102#\"");
  if (!wait_for_reply(CMD::CUSD, AT_OK))
  {
    go = false;
    return nullptr;
  }
  PrintF("Waiting for USSD reply [10secs]\r\n");
  WAIT_FOR(USSD_TIMEOUT);
  while (STILL_WAIT)
  {
    if (strstr(modembuf, "+CUSD:"))
    {
      delay_ms(USSD_REPLY_TIMEOUT);
      break;
    }
  }

  PrintF("Got USSD reply\r\n");
  go = false;
  char *buf = strclone(modembuf);
  if (!buf)
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

  if (!set(CMD::CREG, "2"))
  {
    return false;
  }

  if (!set(CMD::CENG, "1"))
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

  if (!set(CMD::CGREG, "1"))
  {
    return false;
  }
  return true;
}

bool GSM::send_sms(const char *number, const char *text)
{
  rawcmd(CMD::CMGS, CMDMODE::WRITE, number);

  if (!wait_for_reply(CMD::CMGS, AT_INPUT_PROMPT))
  {
    PrintF("No input prompt\r\n");
    return false;
  }
  writestr(text);
  write(0x1A);
  return true;
}

char *GSM::get_sms_by_index(uint32_t num)
{
  rawcmd(CMD::CMGR, CMDMODE::WRITE, num);
  if (!wait_for_reply(CMD::CPMS, AT_OK))
  {
    PrintF("No reply\r\n");
    return nullptr;
  }
  go = false;
  char *sms = extract_sms_body(strclone(modembuf));
  reset();
  return sms;
}

uint32_t GSM::get_account_debet(CELLULAR_OP op)
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
  uint32_t result = 0;
  if (!bal)
  {
    return result;
  }
  *bal = 0;
  while (*(--bal) != ' ' && bal != buf)
    ;
  bal++;
  char *dot = strchr(bal, '.');
  if (dot)
  {
    *dot = result;
  }
  PrintF("Balance: %s\r\n", bal);
  ok = true;

  result = str10_to_word(bal);
  FREE(buf);
  return result;
}

char *GSM::extract_sms_body(char *message)
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
  rawcmd(CMD::CMGD, CMDMODE::WRITE, "1,4");
  if (!wait_for_reply(CMD::CMGD, AT_OK))
  {
    return false;
  }
  return true;
}

uint32_t GSM::get_sms_amount(void)
{
  for (uint32_t i = 1; i < 50; ++i)
  {
    char *sms = get_sms_by_index(i);
    if (!sms)
    {
      FREE(sms);
      return i - 1;
    }
    FREE(sms);
  }
  return 0;
}

void GSM::restart(void)
{
  rawcmd(CMD::CFUN, CMDMODE::WRITE, "1,1");
}

void GSM::turn_off(void)
{
  rawcmd(CMD::CPOWD, CMDMODE::WRITE, 1);
}

bool GSM::get(CMD::ATCMD cmd)
{
  return wait_for_reply(cmd, AT_OK);
}

bool GSM::set(CMD::ATCMD cmd, const char *value)
{
  rawcmd(cmd, CMDMODE::WRITE, value);
  return wait_for_reply(cmd, AT_OK);
}

bool GSM::is_supported(CMD::ATCMD cmd)
{
  rawcmd(cmd, CMDMODE::CHECK, nullptr);
  return wait_for_reply(cmd, AT_OK);
}

bool GSM::get_cc_info()
{
  rawcmd(CMD::CFUN, CMDMODE::WRITE, "0");
  bool result = wait_for_reply(CMD::CFUN, AT_OK);
  if (result)
  {
    PrintF("CELL ID: %s\r\n", modembuf);
  }
  return result;
}

bool GSM::go_online()
{
  rawcmd(CMD::CIPSHUT, CMDMODE::EXEC, nullptr);
  bool result = wait_for_reply(CMD::CIPSHUT, SHUT_OK);
  if (!result)
  {
    return false;
  }
  PrintF("GPRS off\r\n");

  rawcmd(CMD::CSTT, CMDMODE::WRITE, "\"internet.beeline.ru\", \"beeline\", \"beeline\"");
  result = wait_for_reply(CMD::CSTT, AT_OK);
  if (!result)
  {
    return false;
  }

  rawcmd(CMD::SAPBR, CMDMODE::WRITE, "2,1");
  result = wait_for_reply(CMD::SAPBR, AT_OK);
  if (!result)
  {
    return false;
  }

  rawcmd(CMD::CIPSTART, CMDMODE::WRITE, "\"TCP\", \"mail.ru\", \"80\"");
  result = wait_for_reply(CMD::CIPSTART, AT_OK);
  if (!result)
  {
    return false;
  }

  PrintF("GPRS started\r\n");

  return true;
}

char *GSM::get_ICCID()
{
  return nullptr;
}
