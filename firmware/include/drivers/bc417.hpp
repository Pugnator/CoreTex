#pragma once
#include <global.hpp>
#include "drivers/atmodem.hpp"

typedef enum ATCMD
{
 AT
} ATCMD;

#define MODEM_IN_BUFFER_SIZE 512

class bc417: public ATModem
{
public:
 bc417(short ch, word bd)
: ATModem::ATModem(ch, bd)
 {
  self = this;
  buflen = 0;
  ok = false;
  reset();
 }
 ;
 ~bc417(){};
 bool at_check(void);
};
