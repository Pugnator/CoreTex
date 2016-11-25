#include <global.hpp>
#include <common.hpp>
#include <core/vmmu.hpp>
#include <drivers/console.hpp>
#include <drivers/ov528.hpp>
#include <drivers/xmodem.hpp>

class ov528 *ov528::self = nullptr;

void ov528::ov528isr(void)
{
 if (self->Reg->SR & USART_SR_RXNE) //receive
 {
  BLINK;
  short tmp = self->Reg->DR;
  self->Reg->SR &= ~USART_SR_RXNE;
  if (!self->pictransfer && self->buf_size < sizeof self->buf)
  {
   self->buf[self->buf_size++] = tmp;
  }
  else if (self->pictransfer)
  {
   self->pic_size++;
   self->imageblk[self->imageblk_size++] = tmp;
  }
 }
 else if (self->Reg->SR & USART_SR_TC) //transfer
 {
  self->Reg->SR &= ~USART_SR_TC;
 }
}

void ov528::docmd(uint8_t *cmd)
{
 buf_size = 0;
 memset((void*) buf, 0, sizeof buf);
 for (short i = 0; i < CMD_SIZE; i++)
 {
  write(cmd[i]);
 }
}

void ov528::wait_reply(void)
{
 WAIT_FOR(100);
 do
 {
  if (buf_size >= CMD_SIZE * 2)
  {
   return;
  }
 }
 while (STILL_WAIT);
}

bool ov528::wakeup(void)
{
 uint8_t wake[] =
 {
   0xaa, 0x0d, 0, 0, 0, 0
 };
 uint8_t ack[] =
 {
   0xaa, 0x0e, 0x0d, 0, 0, 0
 };
 for (word i = 0; i < WAKEUP_RETRY_COUNT; ++i)
 {
  docmd(wake);
  wait_reply();

  if (0 == memcmp((void *) buf, ack, sizeof ack))
  {
   for (word j = 0; j < sizeof ack; j++)
   {
    write(ack[j]);
   }
   return true;
  }
 }
 return false;
}

bool ov528::setup(void)
{
 uint8_t cmd[] =
 {
   0xaa, 0x1, 0, 0x7, 0, 0x7
 };

 uint8_t ack[] =
 {
   0xaa, 0x0e, 0x01
 };
 docmd(cmd);
 wait_reply();

 if (0 == memcmp((void *) buf, ack, sizeof ack))
 {
  return true;
 }
 return false;
}

bool ov528::snapshot(void)
{
 uint8_t cmd[] =
 {
   0xaa, 0x5, 0, 0, 0, 0
 };

 uint8_t ack[] =
 {
   0xaa, 0x0e, 0x05
 };
 docmd(cmd);
 wait_reply();

 if (0 == memcmp((void *) buf, ack, sizeof ack))
 {
  return true;
 }
 return false;
}

bool ov528::set_packet_size(void)
{
 uint8_t cmd[] =
 {
   0xaa, 0x6, 0x8, 0, 0x4, 0
 };

 uint8_t ack[] =
 {
   0xaa, 0x0e, 0x06
 };
 docmd(cmd);
 wait_reply();

 if (0 == memcmp((void *) buf, ack, sizeof ack))
 {
  return true;
 }
 return false;
}

bool ov528::request_picture(void)
{
 uint8_t cmd[] =
 {
   0xaa, 0x4, 0x1, 0, 0, 0
 };

 uint8_t ack[] =
 {
   0xaa, 0x0e, 0x04
 };
 docmd(cmd);
 wait_reply();

 if (0 == memcmp((void *) buf, ack, sizeof ack))
 {
  return true;
 }
 return false;
}

void ov528::hard_reset(void)
{
 uint8_t cmd[] =
 {
   0xaa, 0x8, 0x0, 0, 0x0, 0
 };

 docmd(cmd);
}

void ov528::soft_reset(void)
{
 uint8_t cmd[] =
 {
   0xaa, 0x8, 0x1, 0, 0x0, 0
 };

 docmd(cmd);
}

bool ov528::sleep(void)
{
 uint8_t cmd[] =
 {
   0xaa, 0x9, 0, 0, 0, 0
 };

 uint8_t ack[] =
 {
   0xaa, 0x0e, 0x09
 };
 docmd(cmd);
 wait_reply();

 if (0 == memcmp((void *) buf, ack, sizeof ack))
 {
  return true;
 }
 return false;
}
void ov528::transfer(void)
{
 pictransfer = true;
 pic_size = 0;
 uint8_t cmd[] =
 {
   0xaa, 0xe, 0, 0, 0, 0
 };

 uint8_t stop[] =
 {
   0xaa, 0xe, 0, 0, 0xf, 0xf
 };
 imageblk = (uint8_t*)stalloc(1100);
 xmodem mdm(1, 9600);
 docmd(cmd);
 word prev_packet_size = 0;
 word byte_cnt = 0;
 for (word i = 0; i < 255; ++i)
 {
  imageblk_size = 0;
  prev_packet_size = pic_size;
  cmd[4] = i;
  docmd(cmd);
  for(;;)
  {
   word tmp = imageblk_size;
   delay_ms(25);
   if(tmp == imageblk_size)
   {
    break;
   }
  }
  /* TRANSFER */

  if(0==i)
  {
   mdm.block_tx((uint8_t*)imageblk + 10, i + 1);
  }
  else
  {
   mdm.block_tx((uint8_t*)imageblk + 4, i + 1);
  }


  if (prev_packet_size == pic_size)
  {
   /*
    * first 6 bytes of the very first packet is ack, we should skip it
    * a packet has 4 bytes header and 2 bytes footer             *
    */
    pic_size -= 6;
   pic_size -= (4 + 2) * (i + 1);
   break; //no more packets
  }
 }
 docmd(stop);
 pictransfer = false;
 stfree((void *)imageblk);
}
bool ov528::default_setup(void)
{
 bool res = true;
 res &= wakeup();
 res &= setup();
 res &= set_packet_size();
 return res;
}

