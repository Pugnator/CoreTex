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
 * 2015, 2016
 *******************************************************************************/
#include <core/core.hpp>
#include <core/io_macro.hpp>
#include <core/isr_helper.hpp>
#include <core/usart.hpp>
#include <global.hpp>
#include <queue.hpp>
#include <log.hpp>

#define USART_DEBUG
#ifdef USART_DEBUG
#define DEBUG_LOG PrintF
#else
#define DEBUG_LOG(...)
#endif

/* USART class */

#define UARTirq (USART1_IRQn - 1)
#define _SR Reg->SR
#define _DR Reg->DR

USART::USART(uint32_t ch, uint32_t bd, USART *isrptr)
{
  __disable_irq();
  channel = ch;
  extirq = isrptr;
  memset(outbuf, 0, sizeof outbuf);
  memset(inbuf, 0, sizeof inbuf);
  QueueInit();
  signup();
  init(ch, bd);
  __enable_irq();
}

/* In the destructor we assign IRQ to `default one in order to avoid CPU hang */
USART::~USART(void)
{
  __disable_irq();
  signout();
  __enable_irq();
  __ISB();
}

void USART::disable(void)
{
  switch (channel)
  {
  case 1:
    NVIC_DisableIRQ(USART1_IRQn);
    break;
  case 2:
    NVIC_DisableIRQ(USART2_IRQn);
    break;
  case 3:
    NVIC_DisableIRQ(USART3_IRQn);
    break;
  }
}

void USART::writestr(const char *str)
{
  const char *p = str;
  while (*p)
  {
    while (!(Reg->SR & USART_SR_TXE))
      ;
    Reg->DR = *p++;
  }
  while (Reg->SR & USART_SR_TC)
    ;
}

void USART::write(char ch)
{
  while (!(Reg->SR & USART_SR_TXE))
    ;
  auto tmp = Reg->SR;
  Reg->DR = ch;
  while (Reg->SR & USART_SR_TC)
    ;
}

char USART::read()
{
  while (!(Reg->SR & USART_SR_RXNE))
    ;
  return Reg->DR;
}

void USART::isr(uint32_t address)
{
  DEBUG_LOG("UART ISR entered\r\n");

  if (_SR & USART_SR_RXNE) //receive
  {
    _SR &= ~USART_SR_RXNE;
    volatile uint16_t c = _DR;
    DEBUG_LOG("Read [0x%X]\r\n", c);
    QueuePut((char)c);
  }
  else if (_SR & USART_SR_TC) //transfer
  {
    _SR &= ~USART_SR_TC;
    DEBUG_LOG("Write completed\r\n");
  }
}

void USART::init(char channel, uint32_t baud)
{
  switch (channel)
  {
  case 1:
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    PIN_OUT_ALT_PP(TX1);
    PIN_INPUT_FLOATING(RX1);
    break;
  case 2:
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    PIN_OUT_ALT_PP(TX2);
    PIN_INPUT_FLOATING(RX2);
    break;
  case 3:
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
    PIN_OUT_ALT_PP(TX3);
    PIN_INPUT_FLOATING(RX3);
    break;
  }
  int irqnum = UARTirq + channel;

  Reg->BRR = (CRYSTAL_CLOCK + baud / 2) / baud;

  Reg->CR1 &= ~USART_CR1_M;
  Reg->CR2 &= ~USART_CR2_STOP;

  Reg->CR1 |= USART_CR1_UE |
              USART_CR1_TE |
              USART_CR1_RE |
              USART_CR1_RXNEIE |
              //USART_CR1_TXEIE |
              USART_CR1_PEIE;

  NVIC_EnableIRQ((IRQn_Type)irqnum);
  NVIC_SetPriority((IRQn_Type)irqnum, 3);
}

void USART::signup()
{
  switch (channel)
  {
  case 1:
    Reg = (USART_TypeDef *)USART1_BASE;
    break;
  case 2:
    Reg = (USART_TypeDef *)USART2_BASE;
    break;
  case 3:
    Reg = (USART_TypeDef *)USART3_BASE;
    break;
  default:
    DEBUG_LOG("Unsupported UART channel\r\n");
    return;
  }

  DEBUG_LOG("First UART handler registration 0x%X\r\n", reinterpret_cast<uint32_t>(this));
  HARDWARE_TABLE[USART1_IRQn] = extirq ? (uint32_t)extirq : reinterpret_cast<uint32_t>(this);
}

void USART::signout()
{
  HARDWARE_TABLE[USART1_IRQn] = 0;
}
