#pragma once
#include <core/usart.hpp>

#define PACKET_SIZE 128

#define SOH  0x01
#define STX  0x02
#define EOT  0x04
#define ACK  0x06
#define NAK  0x15
#define ETB  0x17
#define CAN  0x18
#define CTRLZ 0x1A

class xmodem: public USART
{
public:
 xmodem(short ch, uint32_t bd)
: USART::USART(ch, bd, this)
 {
  ok = false;
  ack = false;
  nack = false;
  txmode = false;
  self = this;
 }

 void send_data(uint8_t *data, uint32_t size);
 bool block_tx(uint8_t *data, uint32_t blockn);
protected:
 volatile bool ok;
 volatile bool ack;
 volatile bool nack;
 void end(void);
 bool wait(void);
 bool send_header(char *filename, uint32_t filesize, uint32_t blockn = 0);
 static class xmodem *self;
 static void xmodemisr(void);
 bool txmode;
 uint16_t crc16(uint8_t *data, uint32_t size);
};

