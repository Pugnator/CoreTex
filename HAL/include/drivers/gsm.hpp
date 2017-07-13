#pragma once

#include "drivers/atmodem.hpp"

class GSM : public ATMODEM
{
public:
 GSM (short ch, word bd) :
   ATMODEM::ATMODEM (ch, bd)
 {
  ok = false;
  reset ();
  sync_speed ();
 }

 bool
 set (CMD::ATCMD cmd, const char* value);
 bool
 get (CMD::ATCMD cmd);
 bool
 is_supported (CMD::ATCMD cmd);
 bool
 setup (void);
 void
 restart (void);
 void
 turn_off (void);

 bool
 send_sms (const char* number, const char* text);
 char*
 get_sms_by_index (word num);
 bool
 delete_all_sms (void);
 word
 get_sms_amount (void);
 char*
 ussd (const char *request);
 word
 get_account_debet (CELLULAR_OP op);
 bool
 get_cc_info ();
private:
 char*
 extract_sms_body (char *message);
 void
 parse_sms (char *message);
 void
 sync_speed (void);

private:
 word cellid;
 word lac;
};
