#pragma once
#include <global.hpp>
#include "drivers/atmodem.hpp"

#define REPLY_TIMEOUT 2000

typedef enum BAUD_RATE
{
 BAUD1 = 1,
 BAUD2 = 2,
 BAUD3 = 3,
 BAUD4 = 4,
 BAUD5 = 5,
 BAUD6 = 6,
 BAUD7 = 7
}BAUD_RATE;

class bc417: public ATModem
{
public:
 bc417(short ch, word bd)
: ATModem::ATModem(ch, bd)
 {
	connected = false;
  self = this;
  buflen = 0;
  ok = false;
  use_ending(false);
  reset();
 };
 ~bc417(){};
 void set_pin(const char* pin);
 void set_name(const char* name);
 void set_baud(BAUD_RATE speed);
 void is_connected();
private:
 bool connected;
};
