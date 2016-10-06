#pragma once
#include <hal/usart.hpp>

using namespace HAL;
using namespace UART;

namespace XMODEM
{

#define PACKET_SIZE 128

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define ETB  0x17
#define CAN  0x18
#define CTRLZ 0x1A

class xmodem: public Uart
  {
public:
  xmodem(short ch, word bd)
      : Uart::Uart(ch, bd, &xmodemisr)
    {
      ok = false;
      ack = false;
      nack = false;
      txmode = false;
      self = this;
    }

  void send_data(uint8_t *data, word size);
  bool block_tx(uint8_t *data, word blockn);
protected:
  volatile bool ok;
  volatile bool ack;
  volatile bool nack;
  void end(void);
  bool wait(void);
  bool send_header(char *filename, word filesize, word blockn = 0);
  static class xmodem *self;
  static void xmodemisr(void);
  bool txmode;
  uint16_t crc16(uint8_t *data, word size);
  };
}
