#pragma once

#include <global.hpp>
#include <core/usart.hpp>
#include <drivers/storage/fatdisk.hpp>

#define CMD_SIZE 6
#define WAKEUP_RETRY_COUNT 100

class ov528: public Uart
{
public:
 ov528(short ch, FATdisk& _disk)
: Uart::Uart(ch, 115200, &ov528isr),
  imageblk_size (0),
  imageblk (nullptr),
  pictransfer (false),
  buf_size (0),
  pic_size (0),
  expected_pic_size(0),
  disk(_disk)
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
 void start_transfer(void);
 bool default_setup(void);
 uint8_t* next_block(word *size);
 word get_block_size();
 static class ov528 *self;
 static void ov528isr(void);

private:
 volatile word imageblk_size;
 volatile uint8_t *imageblk;
 volatile bool pictransfer;
 volatile uint8_t buf_size;
 volatile word pic_size;
 word expected_pic_size;
 volatile uint8_t buf[16];
 FATdisk& disk;


 void wait_reply(word expected = 0);
 void buf_reset(void);
 void docmd(uint8_t *cmd);
 void ack(void);
};
