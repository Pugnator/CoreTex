#pragma once
#include <global.hpp>
#include <drivers/console.hpp>
#include <core/usart.hpp>

using namespace UART;

namespace BC470
{

typedef enum ATRESPONSE
  {
  AT_OK,
  AT_ERROR
  } ATRESPONSE;

typedef enum ATCMD
  {
  AT
  } ATCMD;

#define MODEM_IN_BUFFER_SIZE 512

class bc470: public Uart
  {
public:
  bc470(short ch, word bd)
      : Uart::Uart(ch, bd, &bc470isr)
    {
      self = this;
      buflen = 0;
      go = false;
      ok = false;
    }
  ;
  ~bc470();
  bool check(void);
  static void bc470isr(void);
  static class bc470 *self;
  bool factory_default(void);
  void rawcmd(ATCMD cmd, const char* arg);
  char modembuf[MODEM_IN_BUFFER_SIZE + 1];
  short buflen;
  bool ok;
protected:
  void reset(void);
  bool wait4reply(word timeout = 100);
  bool go;
  };
}
