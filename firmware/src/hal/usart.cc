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
#include <hal/core.hpp>
#include <hal/isr_helper.hpp>
#include <global.hpp>
#include <hal/io_macro.hpp>
#include <hal/usart.hpp>

/* UART class */

#define UARTirq (USART1_IRQn - 1)

using namespace HAL;
using namespace UART;

/* Pointer to the USART object itself in order to be accessible from within a static method */
class Uart *Uart::self = nullptr;

Uart::Uart(word ch, word bd)
  {
    __disable_irq();
    self = this;
    channel = ch;
    switch (channel)
      {
      case 1:
        Reg = (USART_TypeDef*) USART1_BASE;
        IRQ_VECTOR_TABLE[USART1_IRQn + IRQ0_EX] = (word) &isr;
        break;
      case 2:
        Reg = (USART_TypeDef*) USART2_BASE;
        IRQ_VECTOR_TABLE[USART2_IRQn + IRQ0_EX] = (word) &isr;
        break;
      case 3:
        Reg = (USART_TypeDef*) USART3_BASE;
        IRQ_VECTOR_TABLE[USART3_IRQn + IRQ0_EX] = (word) &isr;
        break;
      default:
        //ERROR
        ;
      }
    init(ch, bd);
    __enable_irq();
  }

Uart::Uart(word ch, word bd, void (*isrptr)())
  {
    __disable_irq();
    self = this;
    channel = ch;
    switch (channel)
      {
      case 1:
        Reg = (USART_TypeDef*) USART1_BASE;
        IRQ_VECTOR_TABLE[USART1_IRQn + IRQ0_EX] = (word)isrptr;
        break;
      case 2:
        Reg = (USART_TypeDef*) USART2_BASE;
        IRQ_VECTOR_TABLE[USART2_IRQn + IRQ0_EX] = (word)isrptr;
        break;
      case 3:
        Reg = (USART_TypeDef*) USART3_BASE;
        IRQ_VECTOR_TABLE[USART3_IRQn + IRQ0_EX] = (word)isrptr;
        break;
      default:
        //TODO: Make an adequate error here
        ;
      }
    init(ch, bd);
    isrptr ? __enable_irq() : __disable_irq();
  }

/* In the destructor we assign IRQ to default one in order to avoid CPU hang */
Uart::~Uart(void)
  {
    __disable_irq();
    switch (channel)
      {
      case 1:
        IRQ_VECTOR_TABLE[USART1_IRQn + IRQ0_EX] = (word) &USART1_IRQHandler;
        break;
      case 2:
        IRQ_VECTOR_TABLE[USART2_IRQn + IRQ0_EX] = (word) &USART2_IRQHandler;
        break;
      case 3:
        IRQ_VECTOR_TABLE[USART3_IRQn + IRQ0_EX] = (word) &USART3_IRQHandler;
        break;
      default:
        //ERROR
        ;
      }
    __enable_irq();
    __ISB();
  }

const char* Uart::name()
  {
    return USART_DRIVER_DISPLAY_NAME;
  }

void Uart::disable(void)
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

void Uart::writestr(const char* str)
  {
    const char *p = str;
    while(*p)
      {
        while (!(Reg->SR & USART_SR_TC))
              ;
            Reg->DR = *p++;
      }
  }

void Uart::write(char ch)
  {
    while (!(Reg->SR & USART_SR_TC))
      ;
    Reg->DR = ch;
  }

void Uart::isr(void)
  {
    if (self->Reg->SR & USART_SR_RXNE) //receive
      {
        BLINK;
        self->Reg->SR &= ~USART_SR_RXNE;
      }
    else if (self->Reg->SR & USART_SR_TC) //transfer
      {
        self->Reg->SR &= ~USART_SR_TC;
      }
  }

void Uart::init(char channel, word baud)
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
    volatile int irqnum = UARTirq + channel;
    Reg->BRR = ( CRYSTAL_CLOCK + baud / 2) / baud;
    Reg->CR1 &= ~USART_CR1_M;
    Reg->CR2 &= ~USART_CR2_STOP;
    Reg->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
    NVIC_EnableIRQ((IRQn_Type) irqnum);
    NVIC_SetPriority((IRQn_Type) irqnum, 3);
  }
