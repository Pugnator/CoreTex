#include <global.hpp>
#include <common.hpp>
#include <drivers/gsm.hpp>
#include <hal/isr_helper.hpp>
#include <hal/vmmu.hpp>
#include <stdarg.h>

using namespace HAL;
namespace MODEM
{

static const char *SIM900ATCMD_TEXT[] =
  {
  FOREACH_ATCMD(GENERATE_ATCMD_STRING)
  };

//TODO: may be should add \r\n at runtime to save some space
static const char *RESPONSE_TEXT[] =
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

bool Modem::get(CMD::ATCMD cmd)
  {
    return wait_for_reply(cmd, AT_OK);
  }

bool Modem::set(CMD::ATCMD cmd, const char* value)
  {
    rawcmd(cmd, MODEM::CMDMODE::SET, value);
    return wait_for_reply(cmd, AT_OK);
  }

bool Modem::is_supported(CMD::ATCMD cmd)
  {
    rawcmd(cmd, CMDMODE::CHECK, nullptr);
    return wait_for_reply(cmd, AT_OK);
  }

bool Modem::wait_for_reply(CMD::ATCMD cmd, ATRESPONSE expected, word timeout)
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
            ok = false;
            return ok;
          }
        //do we hit the response we expect?
        else if (strstr(buf, RESPONSE_TEXT[expected]))
          {
            if (CMD::AT == cmd)
              {
                FREE(buf);
                return ok = true;
              }
            const char delim[] = "\r\n";
            char *p = strtok(buf, delim);
            char cmdstr[16] =
              {
              0
              };
            strcpy(cmdstr, "AT+");
            strcat(cmdstr, SIM900ATCMD_TEXT[cmd]);
            if (strstr(buf, cmdstr))
              {
                FREE(buf);
                return ok = true;
              }
            else
              {
                FREE(buf);
                return !(ok = false);
              }
          }
      }
    while (STILL_WAIT);
    FREE(buf);
    return ok;
  }

void Modem::rawcmd(CMD::ATCMD cmd, CMDMODE::MODE mode, const char* arg)
  {
    reset();
    if (CMD::AT != cmd)
      {
        writestr("AT+");
      }
    writestr(SIM900ATCMD_TEXT[cmd]);
    switch (mode)
      {
      case CMDMODE::CHECK:
        writestr("=?");
        break;
      case CMDMODE::SET:
        write('=');
        writestr(arg);
        break;
      case CMDMODE::GET:
        break;
      case CMDMODE::EXEC:
        break;
      default:
        //TODO: errors out here
        break;
      }
    writestr("\r\n");
  }

void Modem::rawcmd(CMD::ATCMD cmd, CMDMODE::MODE mode, word arg)
  {
    //XXX: fix it
    char result[2] =
      {
      0
      };
    result[0] = '0' + arg;
    rawcmd(cmd, mode, result);
  }

}
