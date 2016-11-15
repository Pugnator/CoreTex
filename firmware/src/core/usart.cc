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

#define UARTirq (USART1_IRQn - 1)

using namespace UART;

/* Pointer to the USART object itself in order to be accessible from within a static method */
class Uart *Uart::self = nullptr;

Uart::Uart(word ch, word bd)
{
 __disable_irq();
 self = this;
 channel = ch;
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
 IRQ_VECTOR_TABLE[DMA1_Channel1_IRQn + IRQ0_EX] = (word) &dma;

 RCC->AHBENR |= RCC_AHBENR_DMA1EN;
 DMA1_Channel1->CPAR  =  (word)&USART1->DR;
 DMA1_Channel1->CMAR  =  (word)&outbuf[0];
 DMA1_Channel1->CNDTR =  32;                      //количество данных дл€ обмена


 DMA1_Channel1->CCR   =  0;                       //предочистка регистра конфигурации
 DMA1_Channel1->CCR  &= ~DMA_CCR1_CIRC;           //выключить циклический режим
 DMA1_Channel1->CCR  |=  DMA_CCR1_DIR;            //направление: чтение из пам€ти
 //Ќастроить работу с переферийным устройством
 DMA1_Channel1->CCR  &= ~DMA_CCR1_PSIZE;          //размерность данных 8 бит
 DMA1_Channel1->CCR  &= ~DMA_CCR1_PINC;           //неиспользовать инкремент указател€
 //Ќастроить работу с пам€тью
 DMA1_Channel1->CCR  &= ~DMA_CCR1_MSIZE;          //размерность данных 8 бит
 DMA1_Channel1->CCR  |=  DMA_CCR1_MINC;           //использовать инкремент указател€
 USART1->CR3         |=  USART_CR3_DMAT;          //разрешить передачу USART1 через DMA

 NVIC_EnableIRQ(DMA1_Channel1_IRQn);
 NVIC_SetPriority(DMA1_Channel1_IRQn, 4);
 __enable_irq();
 __ISB();
}

void Uart::dma_go(word size)
{
  DMA1_Channel1->CCR  &= ~DMA_CCR1_EN;      //запретить работу канала
  DMA1_Channel1->CNDTR =  size;      //загрузить количество данных дл€ обмена
  DMA1->IFCR          |=  DMA_IFCR_CTCIF1;  //сбросить флаг окончани€ обмена
  DMA1_Channel1->CCR  |=  DMA_CCR1_EN;      //разрешить работу канала
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

void Uart::dma(void)
{
 SEGGER_RTT_printf(0, "DMA ISR\r\n");
 if(DMA1->ISR & DMA_ISR_TCIF4)
 {
  DMA1->IFCR |= DMA_ISR_TCIF4;
 }
}

void Uart::isr(void)
{
 PIN_HI(LED);
 volatile uint16_t __SR = self->Reg->SR;
 if (__SR & USART_SR_RXNE) //receive
 {
  __SR &= ~USART_SR_RXNE;
  volatile uint16_t c = USART1->DR;
  SEGGER_RTT_printf(0, "Read [0x%X]\r\n", c);
  QueuePut((char)c);
 }
 else if (__SR & USART_SR_TC) //transfer
 {
  __SR &= ~USART_SR_TC;
  SEGGER_RTT_printf(0, "Write completed\r\n");
 }
 else if (__SR & USART_SR_CTS)
 {
  __SR &= ~USART_SR_CTS;
 }
 else if (__SR & USART_SR_FE)
 {
  __SR &= ~USART_SR_FE;
 }
 else if (__SR & USART_SR_IDLE)
 {
  __SR &= ~USART_SR_IDLE;
 }
 else if (__SR & USART_SR_LBD)
 {
  __SR &= ~USART_SR_LBD;
 }
 else if (__SR & USART_SR_NE)
 {
  __SR &= ~USART_SR_NE;
 }
 else if (__SR & USART_SR_ORE)
 {
  __SR &= ~USART_SR_ORE;
 }
 else if (__SR & USART_SR_PE)
 {
  __SR &= ~USART_SR_PE;
 }
 else if (__SR & USART_SR_TXE)
 {
  __SR &= ~USART_SR_TXE;
 }
 self->Reg->SR = __SR;
 PIN_LOW(LED);
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
