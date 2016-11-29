#include <global.hpp>
#include <common.hpp>
#include <core/isr_helper.hpp>
#include <core/vmmu.hpp>
#include <stdarg.h>
#include <log.hpp>
#include "drivers/atmodem.hpp"

class ATModem *ATModem::self = nullptr;

//TODO: may be should add \r\n at runtime to save some space
static const char *RESPONSE_TEXT_CRLF[] =
{
  "\r\nOK\r\n",
  "\r\nCONNECT\r\n",
  "\r\nRING\r\n",
  "\r\nNO CARRIER\r\n",
  "\r\nERROR\r\n",
  "\r\n", //Any response from SIM900
  "\r\nNO DIALTONE\r\n",
  "\r\nBUSY\r\n",
  "\r\nNO ANSWER\r\n",
  "\r\nCall Ready\r\n",
  "\r\nNORMAL POWER DOWN\r\n",
  "",
  "\r\n>",
  "\r\nREADY\r\n"
};

static const char *RESPONSE_TEXT[] =
{
  "OK",
  "CONNECT",
  "RING",
  "NO CARRIER",
  "ERROR",
  "", //Any response from SIM900
  "NO DIALTONE",
  "BUSY",
  "NO ANSWER",
  "Call Ready",
  "NORMAL POWER DOWN",
  "",
  ">",
  "READY"
};

static const char *ATCMD_TEXT[] =
{
  FOREACH_ATCMD(GENERATE_ATCMD_STRING)
};

const char *ATModem::get_cmd_str(CMD::ATCMD cmd)
{
 return ATCMD_TEXT[cmd];
}

void ATModem::procisr(void)
{
 volatile uint16_t __SR = self->Reg->SR;
 if (__SR & USART_SR_RXNE)
 {
  __SR &= ~USART_SR_RXNE;
  volatile uint16_t a = self->Reg->DR;
  SEGGER_RTT_printf(0, "%c", isprint(a) ? a : '?');
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

void ATModem::reset(void)
{
 buflen = 0;
 memset(modembuf, 0, sizeof modembuf);
 go = true;
}

void ATModem::rawcmd(CMD::ATCMD cmd, CMDMODE::MODE mode, const char* arg)
{
 reset();
 if (CMD::AT != cmd)
 {
  writestr("AT+");
 }
 writestr(ATCMD_TEXT[cmd]);
 switch (mode)
 {
  case CMDMODE::CHECK:
   writestr("?");
   break;
  case CMDMODE::SET:
   write('=');
   writestr(arg);
   break;
  case CMDMODE::GET:
   break;
  case CMDMODE::EXEC:
   break;
  case CMDMODE::RAW:
   if(arg)
   {
    writestr(arg);
   }
   break;
  default:
   //TODO: errors out here
   break;
 }
 if(CMD::CMGS == cmd)
 {
  write('\r');
 }
 else if(crlf_end)
 {
  writestr("\r\n");
 }

}

void ATModem::rawcmd(CMD::ATCMD cmd, CMDMODE::MODE mode, word arg)
{
 //XXX: fix it
 char result[2] =
 {
   0
 };
 result[0] = '0' + arg;
 rawcmd(cmd, mode, result);
}

bool ATModem::wait_for_reply(CMD::ATCMD cmd, ATRESPONSE expected, word timeout)
{
 ok = false;
 char buf[MODEM_IN_BUFFER_SIZE + 1] = {0};

 WAIT_FOR(timeout);
 do
 {
  memcpy(buf, modembuf, sizeof modembuf);
  //explicit check for error
  if (strstr(buf, RESPONSE_TEXT_CRLF[AT_ERROR]))
  {
   SEGGER_RTT_WriteString(0, "SIM900: ERROR\r\n");
   ok = false;
   return ok;
  }
  //do we hit the response we expect?
  else if (strstr(buf, RESPONSE_TEXT_CRLF[expected]))
  {
   if (CMD::AT == cmd)
   {
    SEGGER_RTT_WriteString(0, "SIM900: Echo OK\r\n");
    return ok = true;
   }
   const char delim[] = "\r\n";
   char *p = strtok(buf, delim);
   char cmdstr[16] =
   {
     0
   };
   strcpy(cmdstr, "AT+");
   strcat(cmdstr, get_cmd_str(cmd));
   if (strstr(buf, cmdstr))
   {
    return ok = true;
   }
   else
   {
    SEGGER_RTT_printf(0, "SIM900: ERR [%s]\r\n", buf);
    return !(ok = false);
   }
  }
 }
 while (STILL_WAIT);
 return ok;
}

bool ATModem::wait_for_reply_noend(CMD::ATCMD cmd, ATRESPONSE expected, word timeout)
{
 ok = false;
 char buf[MODEM_IN_BUFFER_SIZE + 1] = {0};

 WAIT_FOR(timeout);
 do
 {
  memcpy(buf, modembuf, sizeof modembuf);
  //explicit check for error
  if (strstr(buf, RESPONSE_TEXT[AT_ERROR]))
  {
   SEGGER_RTT_WriteString(0, "ERROR\r\n");
   ok = false;
   return ok;
  }
  //do we hit the response we expect?
  else if (strstr(buf, RESPONSE_TEXT[expected]))
  {
   if (CMD::AT == cmd)
   {
    SEGGER_RTT_WriteString(0, "Echo OK\r\n");
    return ok = true;
   }
   const char delim[] = "\r\n";
   char *p = strtok(buf, delim);
   char cmdstr[16] =   {0};
   strcpy(cmdstr, "AT+");
   strcat(cmdstr, get_cmd_str(cmd));
   if (strstr(buf, cmdstr))
   {
    return ok = true;
   }
   else
   {
    SEGGER_RTT_printf(0, "ERR [%s]\r\n", buf);
    return !(ok = false);
   }
  }
 }
 while (STILL_WAIT);
 return ok;
}
