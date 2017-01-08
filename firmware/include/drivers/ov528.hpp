#pragma once

#include <global.hpp>
#include <core/usart.hpp>
#include <drivers/console.hpp>

#define CMD_SIZE 6
#define WAKEUP_RETRY_COUNT 100

class ov528: public Uart
{
public:
 ov528(short ch, Console *debug = nullptr)
: Uart::Uart(ch, 115200, &ov528isr), conout(debug)
 {
  self = this;
  imageblk = nullptr;
  imageblk_size = 0;
  pictransfer = false;
  memset((void *)buf, 0, sizeof buf);
  buf_size = 0;
  pic_size = 0;
 }

 bool wakeup(void);
 bool sleep(void);
 bool set_packet_size(void);
 bool snapshot(void);
 bool setup(void);
 void hard_reset(void);
 void soft_reset(void);
 bool request_picture(void);
 void transfer(void);
 bool default_setup(void);
 static class ov528 *self;
 static void ov528isr(void);
 volatile bool pictransfer;
 volatile uint8_t buf[16];
 volatile uint8_t buf_size;
private:
 volatile uint8_t *imageblk;
 volatile word imageblk_size;
 volatile word pic_size;
 void wait_reply(void);
 void docmd(uint8_t *cmd);
 void ack(void);
 Console *conout;
};
