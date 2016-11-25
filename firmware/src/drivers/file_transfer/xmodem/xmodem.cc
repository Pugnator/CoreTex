/*******************************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 2015
 *******************************************************************************/
#include <global.hpp>
#include <common.hpp>
#include <drivers/xmodem.hpp>

word write_word(word val, uint8_t *buf)
{
 word ci = 0;
 if (val == 0) {
  // If already zero then just return zero
  buf[ci++] = '0';
 }
 else {
  // Maximum number of decimal digits in uint32_t is 10
  uint8_t s[11];
  int32_t i = sizeof(s)-1;
  s[i] = 0;
  while ((val > 0) && (i > 0)) {
   s[--i] = (val % 10) + '0';
   val /= 10;
  }
  uint8_t *sp = &s[i];
  while (*sp) {
   buf[ci++] = *sp++;
  }
 }
 buf[ci] = 0;
 return ci;
}

class xmodem *xmodem::self = nullptr;

void xmodem::xmodemisr(void)
{
 if (self->Reg->SR & USART_SR_RXNE) //receive
 {
  short tmp = self->Reg->DR;
  self->Reg->SR &= ~USART_SR_RXNE;
  if (self->txmode)
  {
   switch (tmp)
   {
    case 'c':
    case 'C':
    self->ok = true;
    break;
    case ACK:
     self->ack = true;
     break;
    case NAK:
     self->nack = true;
     break;
    default:
     break;
   }
  }
 }
 else if (self->Reg->SR & USART_SR_TC) //transfer
 {
  self->Reg->SR &= ~USART_SR_TC;
 }
}

uint16_t xmodem::crc16(uint8_t *buf, word size)
{
 uint16_t crc = 0;
 for (word i = 0; i < size; ++i)
 {
  crc ^= (uint16_t) *buf++ << 8;
  for (word i = 0; i < 8; ++i)
  {
   if (crc & 0x8000)
   {
    crc = crc << 1 ^ 0x1021;
   }
   else
   {
    crc <<= 1;
   }
  }
 }
 return crc;
}
bool xmodem::block_tx(uint8_t *data, word blockn)
{
 txmode = true;
 ack = false;
 nack = false;
 uint16_t crc = crc16(data, PACKET_SIZE);
 write(SOH); //SOH for 128byte and STX for 1k
 write(blockn & 0xFF);
 write(~blockn & 0xFF);
 for (word i = 0; i < PACKET_SIZE; ++i)
 {
  if(0 == data[i])
  {
   data[i] = 1;
  }
  write(data[i]);
 }
 write(crc >> 8 & 0xFF);
 write(crc & 0xFF);
 WAIT_FOR(500);
 do
 {
  if(ack || nack)
  {
   txmode = false;
   return ack ? true : false;
  }
 }while(STILL_WAIT);
 txmode = false;
 return false;
}
void xmodem::end(void)
{
 write(EOT);
}

bool xmodem::send_header(char *filename, word filesize, word blockn)
{
 txmode = true;
 ack = false;
 nack = false;
 char header[128] =  { 0 };
 memset(header, 0, 128);
 strcpy(header, filename);
 strcat(header + strlen(filename) + 1, "128");
 uint16_t crc = crc16((uint8_t*) header, PACKET_SIZE);
 write(SOH); //SOH for 128byte and STX for 1k
 write(blockn & 0xff);
 write(~blockn & 0xFF);
 for (word i = 0; i < PACKET_SIZE; ++i)
 {
  write(header[i]);
 }
 write(crc >> 8 & 0xFF);
 write(crc & 0xFF);
 WAIT_FOR(500);
 do
 {
  if(ack || nack)
  {
   txmode = false;
   return ack ? true : false;
  }
 }while(STILL_WAIT);
 txmode = false;
 return false;
}

void xmodem::send_data(uint8_t *data, word size)
{
 wait();
 uint8_t bn = 1;
 word curp = 0;
 while(curp + PACKET_SIZE  < size)
 {
  block_tx(data + curp, bn++);
  curp += PACKET_SIZE;
 }
 end();
}

bool xmodem::wait(void)
{
 ok = false;
 txmode = true;
 while(!ok);
 txmode = false;
 return true;
}
