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
} BAUD_RATE;

class BC417 : public ATMODEM
{
public:
  BC417(short ch, uint32_t bd)
      : ATMODEM::ATMODEM(ch, bd)
  {
    connected = false;
    buflen = 0;
    ok = false;
    use_ending(false);
    reset();
  };
  ~BC417(){};
  void set_pin(const char *pin);
  void set_name(const char *name);
  void set_baud(BAUD_RATE speed);
  void is_connected();

private:
  bool connected;
};
