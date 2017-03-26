#pragma once

#include <global.hpp>
#include <core/usart.hpp>
#include <drivers/storage/fatdisk.hpp>

#define CMD_SIZE 6
#define WAKEUP_RETRY_COUNT 100

class ov528: public Uart
{
public:
 ov528(short ch, FATdisk *Disk)
: Uart::Uart(ch, 115200, &ov528isr),
  disk (Disk),
  imageblk (nullptr),
  imageblk_size (0),
  pictransfer (false),
  buf_size (0),
  pic_size (0)
 {
	 self = this;
  memset((void *)buf, 0, sizeof buf);
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

private:
 FATdisk *disk;

 volatile word imageblk_size;
 volatile uint8_t *imageblk;
 volatile bool pictransfer;
 volatile uint8_t buf_size;
 volatile word pic_size;

 volatile uint8_t buf[16];


 void wait_reply(void);
 void docmd(uint8_t *cmd);
 void ack(void);
};
