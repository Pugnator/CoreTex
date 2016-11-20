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

/* UART class */

using namespace UART;

#define UARTirq (USART1_IRQn - 1)
#define _SR self->Reg->SR
#define _DR self->Reg->DR

/* Pointer to the USART object itself in order to be accessible from within a static method */
class Uart *Uart::self = nullptr;

Uart::Uart(word ch, word bd)
{
 __disable_irq();
 self = this;
 channel = ch;
 memset(outbuf, 0xaa, sizeof outbuf);
 memset(inbuf, 0, sizeof inbuf);
 QueueInit();
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
   IRQ_VECTOR_TABLE[USART1_IRQn + IRQ0_EX] = (word) isrptr;
   break;
  case 2:
   Reg = (USART_TypeDef*) USART2_BASE;
   IRQ_VECTOR_TABLE[USART2_IRQn + IRQ0_EX] = (word) isrptr;
   break;
  case 3:
   Reg = (USART_TypeDef*) USART3_BASE;
   IRQ_VECTOR_TABLE[USART3_IRQn + IRQ0_EX] = (word) isrptr;
   break;
  default:
   SEGGER_RTT_printf(0, "Unsupported UART channel\r\n");
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

void Uart::dma_on()
{
 __disable_irq();
 IRQ_VECTOR_TABLE[DMA1_Channel4_IRQn + IRQ0_EX] = (word) &dmatx;
 IRQ_VECTOR_TABLE[DMA1_Channel5_IRQn + IRQ0_EX] = (word) &dmarx;

 RCC->AHBENR |= RCC_AHBENR_DMA1EN;

 //Transmit
 DMA1_Channel4->CCR  &= ~DMA_CCR4_EN;
 DMA1_Channel4->CPAR  =  (word)&Reg->DR;
 DMA1_Channel4->CMAR  =  (word)&outbuf[0];
 DMA1_Channel4->CNDTR =  sizeof outbuf;


 DMA1_Channel4->CCR   =  0;
 DMA1_Channel4->CCR  &= ~DMA_CCR4_CIRC;
 DMA1_Channel4->CCR  |=  DMA_CCR4_DIR;

 DMA1_Channel4->CCR  &=  ~DMA_CCR4_TCIE;

 DMA1_Channel4->CCR  &= ~DMA_CCR4_PSIZE;
 DMA1_Channel4->CCR  &= ~DMA_CCR4_PINC;

 DMA1_Channel4->CCR  &= ~DMA_CCR4_MSIZE;
 DMA1_Channel4->CCR  |=  DMA_CCR4_MINC;
 Reg->CR3         |=  USART_CR3_DMAT;

 NVIC_EnableIRQ(DMA1_Channel4_IRQn);
 NVIC_SetPriority(DMA1_Channel4_IRQn, 4);

 //Receive
 DMA1_Channel5->CCR  &= ~DMA_CCR5_EN;
 DMA1_Channel5->CPAR  =  (word)&Reg->DR;
 DMA1_Channel5->CMAR  =  (word)&inbuf[0];
 DMA1_Channel5->CNDTR =  sizeof inbuf;


 DMA1_Channel5->CCR   =  0;
 DMA1_Channel5->CCR  &= ~DMA_CCR5_CIRC;
 //DMA1_Channel5->CCR  &=  ~DMA_CCR5_DIR;
 DMA1_Channel5->CCR  &=  ~DMA_CCR5_TCIE;
 DMA1_Channel5->CCR  &= ~DMA_CCR5_PSIZE;
 DMA1_Channel5->CCR  &= ~DMA_CCR5_PINC;

 DMA1_Channel5->CCR  &= ~DMA_CCR5_MSIZE;
 DMA1_Channel5->CCR  |=  DMA_CCR5_MINC;
 Reg->CR3         |=  USART_CR3_DMAR;

 NVIC_EnableIRQ(DMA1_Channel5_IRQn);
 NVIC_SetPriority(DMA1_Channel5_IRQn, 4);


 __enable_irq();
 __ISB();
}

void Uart::dma_off()
{
	DMA1_Channel4->CCR = 0;
	DMA1_Channel5->CCR = 0;
	RCC->AHBENR &= ~RCC_AHBENR_DMA1EN;
}

bool Uart::is_dma_on()
{
 return RCC->AHBENR & RCC_AHBENR_DMA1EN;
}

void Uart::dmatx_go(word size)
{
 DMA1_Channel4->CCR  &= ~DMA_CCR4_EN;
 DMA1_Channel4->CNDTR =  size;
 DMA1->IFCR          |=  DMA_IFCR_CTCIF4;
 DMA1_Channel4->CCR  |=  DMA_CCR4_EN;
}

void Uart::dmarx_go(word size)
{
 DMA1_Channel5->CCR  &= ~DMA_CCR5_EN;
 DMA1_Channel5->CNDTR =  size;
 DMA1->IFCR          |=  DMA_IFCR_CTCIF5;
 DMA1_Channel5->CCR  |=  DMA_CCR5_EN;
}

const char*
Uart::name()
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
 while (*p)
 {
  while (!(Reg->SR & USART_SR_TXE));
  Reg->DR = *p++;
 }
 while (Reg->SR & USART_SR_TC);
}

void Uart::write(char ch)
{
 while (!(Reg->SR & USART_SR_TXE));
 Reg->DR = ch;
 while (Reg->SR & USART_SR_TC);
}

char Uart::read()
{
 while (!(Reg->SR & USART_SR_RXNE));
 return Reg->DR;
}

void Uart::dmarx(void)
{
 if(DMA1->ISR & DMA_ISR_TCIF4)
 {
  DMA1->IFCR |= DMA_ISR_TCIF4;
 }
 else if(DMA1->ISR & DMA_ISR_HTIF4)
 {
  DMA1->IFCR |= DMA_ISR_HTIF4;
 }
 else if(DMA1->ISR & DMA_ISR_TEIF4)
 {
  DMA1->IFCR |= DMA_ISR_TEIF4;
 }
}

void Uart::dmatx(void)
{
	if(DMA1->ISR & DMA_ISR_GIF5)
	{
		DMA1->IFCR |= DMA_ISR_GIF5;
	}
	else if(DMA1->ISR & DMA_ISR_TCIF5)
 {
  DMA1->IFCR |= DMA_ISR_TCIF5;
 }
 else if(DMA1->ISR & DMA_ISR_HTIF5)
 {
  DMA1->IFCR |= DMA_ISR_HTIF5;
 }
 else if(DMA1->ISR & DMA_ISR_TEIF5)
 {
  DMA1->IFCR |= DMA_ISR_TEIF5;
 }
 else if(DMA1->ISR & DMA_ISR_GIF5)
 {
  DMA1->IFCR |= DMA_ISR_GIF5;
 }
}

void Uart::isr(void)
{
 if (_SR & USART_SR_RXNE) //receive
 {
  _SR &= ~USART_SR_RXNE;
  volatile uint16_t c = _DR;
  SEGGER_RTT_printf(0, "Read [0x%X]\r\n", c);
  QueuePut((char)c);
 }
 else if (_SR & USART_SR_TC) //transfer
 {
  _SR &= ~USART_SR_TC;
  SEGGER_RTT_printf(0, "Write completed\r\n");
 }
 else if (_SR & USART_SR_CTS)
 {
  _SR &= ~USART_SR_CTS;
 }
 else if (_SR & USART_SR_FE)
 {
  _SR &= ~USART_SR_FE;
  volatile uint16_t tmp = _SR;
  tmp = _DR;
 }
 else if (_SR & USART_SR_IDLE)
 {
  _SR &= ~USART_SR_IDLE;
  volatile uint16_t tmp = _SR;
  tmp = _DR;
  SEGGER_RTT_printf(0, "Idle line\r\n");
 }
 else if (_SR & USART_SR_LBD)
 {
  _SR &= ~USART_SR_LBD;
 }
 else if (_SR & USART_SR_NE)
 {
  _SR &= ~USART_SR_NE;
 }
 else if (_SR & USART_SR_ORE)
 {
  _SR &= ~USART_SR_ORE;
  volatile uint16_t tmp = _SR;
  tmp = _DR;
 }
 else if (_SR & USART_SR_PE)
 {
  _SR &= ~USART_SR_PE;
 }
 else if (_SR & USART_SR_TXE)
 {
  _SR &= ~USART_SR_TXE;
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
 int irqnum = UARTirq + channel;

 Reg->BRR = ( CRYSTAL_CLOCK + baud / 2) / baud;

 Reg->CR1 &= ~USART_CR1_M;
 Reg->CR2 &= ~USART_CR2_STOP;

 volatile uint16_t __CR1 = Reg->CR1;
 __CR1 |=
   USART_CR1_UE |
   USART_CR1_TE |
   USART_CR1_RE |
   USART_CR1_RXNEIE |
   USART_CR1_PEIE;

 Reg->CR1 = __CR1;

 NVIC_EnableIRQ((IRQn_Type) irqnum);
 NVIC_SetPriority((IRQn_Type) irqnum, 3);
}

uint8_t *Uart::get_rx_buf()
{
 return inbuf;
}

uint8_t *Uart::get_tx_buf()
{
 return outbuf;
}
