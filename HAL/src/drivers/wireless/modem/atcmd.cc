#include <global.hpp>
#include <common.hpp>
#include <core/isr_helper.hpp>
#include <core/vmmu.hpp>
#include <stdarg.h>
#include <log.hpp>
#include "drivers/atmodem.hpp"


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
  "\r\nREADY\r\n",
  "\r\n+CME ERROR: operation not allowed\r\n",
  "\r\nSHUT OK\r\n"
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
  "READY",
  "+CME ERROR: operation not allowed",
  "SHUT OK"
};

static const char *ATCMD_TEXT[] =
{
  FOREACH_ATCMD(GENERATE_ATCMD_STRING)
};

const char *ATMODEM::get_cmd_str(CMD::ATCMD cmd)
{
 return ATCMD_TEXT[cmd];
}

void ATMODEM::isr(uint32_t address)
{
 volatile uint16_t __SR = Reg->SR;
 if (__SR & USART_SR_RXNE)
 {
  __SR &= ~USART_SR_RXNE;
  volatile uint16_t a = Reg->DR;
  PrintF("%c", isprint(a) ? a : '?');
  if (!go || buflen >= MODEM_IN_BUFFER_SIZE)
  {
   go = false;
   return;
  }
  modembuf[buflen++] = a;
 }
 else if (__SR & USART_SR_TC)
 {
  __SR &= ~USART_SR_TC;
 }
 Reg->SR = __SR;
}

void ATMODEM::reset(void)
{
 buflen = 0;
 memset(modembuf, 0, sizeof modembuf);
 go = true;
}

void ATMODEM::rawcmd(CMD::ATCMD cmd, CMDMODE::MODE mode, const char* arg)
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
  case CMDMODE::WRITE:
   write('=');
   writestr(arg);
   break;
  case CMDMODE::READ:
   writestr("=?");
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

void ATMODEM::rawcmd(CMD::ATCMD cmd, CMDMODE::MODE mode, uint32_t arg)
{
 //XXX: fix it
 char result[2] =
 {
   0
 };
 result[0] = '0' + arg;
 rawcmd(cmd, mode, result);
}

bool ATMODEM::wait_for_reply(CMD::ATCMD cmd, ATRESPONSE expected, uint32_t timeout)
{
 ok = false;
 char buf[MODEM_IN_BUFFER_SIZE + 1] = {0};
 const char **RESPONSE_TEXT_PTR = crlf_end ? RESPONSE_TEXT_CRLF : RESPONSE_TEXT;
 WAIT_FOR(timeout);
 do
 {
  memcpy(buf, modembuf, sizeof modembuf);
  //explicit check for error
  if (strstr(buf, RESPONSE_TEXT_PTR[AT_ERROR]))
  {
   SEGGER_RTT_WriteString(0, "ERROR\r\n");
   ok = false;
   return ok;
  }

  if (strstr(buf, RESPONSE_TEXT_PTR[CME_ERROR]))
  {
   SEGGER_RTT_WriteString(0, "CME ERROR\r\n");
   ok = false;
   return ok;
  }

  //do we hit the response we expect?
  else if (strstr(buf, RESPONSE_TEXT_PTR[expected]))
  {
   if (CMD::AT == cmd)
   {
    //SEGGER_RTT_WriteString(0, "Echo OK\r\n");
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
    //Found echo reply
    if (strstr(modembuf, RESPONSE_TEXT_PTR[AT_OK]))
    {
    	//Remove trailing OK
    	*strstr(modembuf, RESPONSE_TEXT_PTR[AT_OK]) = 0;
    	//Remove command and it's echo
    	strcpy(cmdstr, "+");
    	strcat(cmdstr, get_cmd_str(cmd));
    	strcat(cmdstr, ":");
    	char *reply = strstr(modembuf, cmdstr);
    	if (reply)
    	{
    		PrintF("Command echo found: %s\r\n", reply);
    		reply += strlen(cmdstr);
    		strcpy(buf, reply);
    		strcpy(modembuf, buf);

    	}
    }
    return ok = true;
   }
   else
   {
    //PrintF("ERR [%s]\r\n", buf);
    return !(ok = false);
   }
  }
 }
 while (STILL_WAIT);
 return ok;
}

void ATMODEM::use_ending(bool mode)
{
 crlf_end = mode;
}
