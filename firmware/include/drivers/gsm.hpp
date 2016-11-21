#pragma once

#include <global.hpp>
#include <core/usart.hpp>
#include <drivers/console.hpp>

using namespace UART;

#define MODEM_IN_BUFFER_SIZE 512
#define SMS_MAX_SIZE 140

namespace MODEM
{

typedef struct smsctx
  {
    bool read;
    char message[SMS_MAX_SIZE];
  }smsctx;

namespace CMD
{
/* Autogenerated AT command list-enumerator */
#define FOREACH_ATCMD(ATCMD) \
	ATCMD(AT)\
    ATCMD(IPR)\
	ATCMD(CGMI)\
	ATCMD(CGMM)\
	ATCMD(CGMN)\
	ATCMD(CGMP)\
	ATCMD(CGMR)\
	ATCMD(CGMS)\
	ATCMD(CGSN)\
	ATCMD(CSCS)\
	ATCMD(CIMI)\
	ATCMD(CCLK)\
	ATCMD(CNUM)\
	ATCMD(CREG)\
	ATCMD(COPS)\
	ATCMD(CPOL)\
	ATCMD(CPLS)\
	ATCMD(CLCK)\
	ATCMD(CPWD)\
	ATCMD(CCUG)\
	ATCMD(CLCC)\
	ATCMD(CUSD)\
	ATCMD(CPAS)\
	ATCMD(CFUN)\
	ATCMD(CPIN)\
	ATCMD(CBC)\
	ATCMD(CIND)\
	ATCMD(CPBS)\
	ATCMD(CPBR)\
	ATCMD(CPBW)\
	ATCMD(CRSM)\
	ATCMD(CMAR)\
	ATCMD(CMEE)\
	ATCMD(CSMS)\
	ATCMD(CPMS)\
	ATCMD(CLIP)\
	ATCMD(CMGF)\
	ATCMD(CSCA)\
	ATCMD(CSMP)\
	ATCMD(CSDH)\
	ATCMD(CSCB)\
	ATCMD(CSAS)\
	ATCMD(CRES)\
	ATCMD(CNMI)\
	ATCMD(CMGL)\
	ATCMD(CMGR)\
	ATCMD(CMGS)\
	ATCMD(CMSS)\
	ATCMD(CMGW)\
	ATCMD(CMGD)\
	ATCMD(CGDCONT)\
	ATCMD(CGDSCONT)\
	ATCMD(CGEQREQ)\
	ATCMD(CGQREQ )\
	ATCMD(CGQMIN )\
	ATCMD(CGEQMIN)\
	ATCMD(CGEQNEG)\
	ATCMD(CGATT )\
	ATCMD(CGACT  )\
	ATCMD(CGCMOD )\
	ATCMD(CGTFT  )\
	ATCMD(CGDATA )\
	ATCMD(CGPADDR)\
	ATCMD(CGCLASS)\
	ATCMD(CGREG )\
	ATCMD(CGSMS  )\
	ATCMD(CPOWD)\
	ATCMD(GSMBUSY)\
	ATCMD(WRONG_COMMAND)

#define GENERATE_ATCMD(ATCMD) ATCMD,
#define GENERATE_ATCMD_STRING(STRING) #STRING,

typedef enum SIM900ATCMD
  {
  FOREACH_ATCMD(GENERATE_ATCMD)
  } ATCMD;
}

namespace CMDMODE
{
typedef enum MODE
  {
  SET,
  GET,
  CHECK,
  EXEC,
  RAW
  } MODE;
}

typedef enum OPERATOR
{
 BEELINE,
 MEGAFON,
 MTS,
 TELE2
}OPERATOR;

typedef enum ATRESPONSE
  {
  AT_OK,
  AT_CONNECT,
  AT_RING,
  AT_NO_CARRIER,
  AT_ERROR,
  AT_STRING,
  AT_NO_DIALTONE,
  AT_BUSY,
  AT_NO_ANSWER,
  SIM900_CALL_READY,
  SIM900_NORMAL_POWER_DOWN,
  AT_NO_REPLY,
  AT_INPUT_PROMPT,
  AT_READY
  } ATRESPONSE;

class Modem: public Uart
  {
public:
  Modem(short ch, word bd)
      : Uart::Uart(ch, bd, &modemisr)
    {
      ok = false;
      go = false;
      Modem::self = this;
      reset();
      sync_speed();
    }

  void reset(void);
  bool set(CMD::ATCMD cmd, const char* value);
  bool get(CMD::ATCMD cmd);
  bool is_supported(CMD::ATCMD cmd);
  bool setup(void);
  void restart(void);
  void turn_off(void);
  void monitor(void);
  void rawcmd(CMD::ATCMD cmd, CMDMODE::MODE mode, const char* arg = nullptr);
  void rawcmd(CMD::ATCMD cmd, CMDMODE::MODE mode, word argument);

  /*GSM functions*/
  bool send_sms(const char* number, const char* text);
  char* get_sms_by_index(word num);
  bool delete_all_sms(void);
  word get_sms_amount(void);
  char* ussd(const char *request);
  word get_account_debet(OPERATOR op);
  /******************************************/


  static void modemisr(void);
  static class Modem *self;
  char modembuf[MODEM_IN_BUFFER_SIZE + 1];
  short buflen;
  bool ok;
protected:
  bool go;
  char* extract_sms_body(char *message);
  bool wait_for_reply(CMD::ATCMD cmd, ATRESPONSE expected, word timeout = 1000);
  void parse_sms(char *message);
  void sync_speed(void);
  };
}
