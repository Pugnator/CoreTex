#include <global.hpp>
#include <common.hpp>
#include <core/vmmu.hpp>
#include <drivers/ov528.hpp>
#include <drivers/xmodem.hpp>
#include <log.hpp>

class ov528 *ov528::self = nullptr;

void ov528::ov528isr(void)
{
 if (self->Reg->SR & USART_SR_RXNE) //receive
 {
  BLINK;
  short tmp = self->Reg->DR;
  self->Reg->SR &= ~USART_SR_RXNE;
  //SEGGER_RTT_printf(0, "0x%X\r\n", tmp);
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
	SEGGER_RTT_WriteString(0, "Waking camera up...\r\n");
  docmd(wake);
  wait_reply();

  if (0 == memcmp((void *) buf, ack, sizeof ack))
  {
   SEGGER_RTT_WriteString(0, "Camera answered\r\n");
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
	 SEGGER_RTT_WriteString(0, "Camera was set up\r\n");
  return true;
 }
 return false;
}

bool ov528::snapshot(void)
{
	SEGGER_RTT_WriteString(0, "Image snapshot\r\n");
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
	 SEGGER_RTT_WriteString(0, "Snapshot OK\r\n");
  return true;
 }
 return false;
}

bool ov528::set_packet_size(void)
{
	SEGGER_RTT_WriteString(0, "Setting packet size\r\n");
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
	SEGGER_RTT_WriteString(0, "Packet size set OK\r\n");
  return true;
 }
 return false;
}

bool ov528::request_picture(void)
{
	SEGGER_RTT_WriteString(0, "Request a picture\r\n");
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
	 SEGGER_RTT_WriteString(0, "Image request OK\r\n");
  return true;
 }
 return false;
}

void ov528::hard_reset(void)
{
 SEGGER_RTT_WriteString(0, "Camera hard reset\r\n");
 uint8_t cmd[] =
 {
   0xaa, 0x8, 0x0, 0, 0x0, 0
 };

 docmd(cmd);
}

void ov528::soft_reset(void)
{
	SEGGER_RTT_WriteString(0, "Camera soft reset\r\n");
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
 SEGGER_RTT_WriteString(0, "Starting image transfer\r\n");
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
 volatile uint8_t image[1100] = {0};
 imageblk = image;
 docmd(cmd);
 word prev_packet_size = 0;
 word byte_cnt = 0;
 FATdisk disk(1);
 	FATFS fs;
 	FRESULT res =	disk.mount(&fs, "0:", 1);
 	if (FR_OK != res)
 	 {
 		 SEGGER_RTT_WriteString(0, "Failed to mound disk\r\n");
 		 return;
 	 }
 FIL pic;
 res = disk.open(&pic, "image.jpg", FA_WRITE | FA_CREATE_ALWAYS);
 if (FR_OK != res)
 {
	 SEGGER_RTT_WriteString(0, "Failed to create image file\r\n");
	 return;
 }
 unsigned written = 0;
 for (word i = 0; i < 255; ++i)
 {
	SEGGER_RTT_printf(0, "Image block %u [size: %u]\r\n", i, pic_size);
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
  SEGGER_RTT_WriteString(0, "Writing image block\r\n");
  res = disk.f_write(&pic, (void*)imageblk + 4, imageblk_size - 6, &written);
  if (FR_OK != res)
  {
  	 SEGGER_RTT_WriteString(0, "Failed to write the image block\r\n");
  	return;
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
 disk.flush(&pic);
 disk.close(&pic);
}

bool ov528::default_setup(void)
{
 bool res = true;
 res &= wakeup();
 res &= setup();
 res &= set_packet_size();
 SEGGER_RTT_printf(0, "Camera setup %s\r\n", res ? "OK" : "FAILED");
 return res;
}

