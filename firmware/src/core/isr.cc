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

#include <global.hpp>
#include <common.hpp>
#include <log.hpp>
#include <core/io_macro.hpp>
#include <core/isr_helper.hpp>
#include <drivers/storage/fatdisk.hpp>

volatile word tickcounter = 0;
volatile word timerms = 0;

extern "C"
{
void SysTick_Handler(void)
{
 if (tickcounter)
 {
  --tickcounter;
 }
 if (timerms)
	 {
		 ++timerms;
	 }
}

void SPI1_IRQHandler(void)
{
 if ( SPI1->SR & SPI_SR_RXNE) //receive
 {
  short c = SPI1->DR;
  SPI1->SR &= ~SPI_SR_RXNE;
 }
 else if ( SPI1->SR & SPI_SR_TXE) //transfer
 {
  SPI1->SR &= ~SPI_SR_TXE;
 }
}

void SPI2_IRQHandler(void)
{
 if ( SPI1->SR & SPI_SR_RXNE) //receive
 {
  short c = SPI1->DR;
  SPI1->SR &= ~SPI_SR_RXNE;
 }
 else if ( SPI1->SR & SPI_SR_TXE) //transfer
 {
  SPI1->SR &= ~SPI_SR_TXE;
 }
}

void USART1_IRQHandler(void)
{
 if ( USART1->SR & USART_SR_RXNE) //receive
 {
  short c = USART1->DR;
  USART1->SR &= ~USART_SR_RXNE;
  ;
 }
 else if ( USART1->SR & USART_SR_TC) //transfer
 {
  USART1->SR &= ~USART_SR_TC;
 }
}

/* GSM port */

void USART2_IRQHandler(void)
{
 if ( USART2->SR & USART_SR_RXNE) //receive
 {
  char c = USART2->DR;
  USART2->SR &= ~USART_SR_RXNE;
 }
 else if ( USART2->SR & USART_SR_TC) //transfer
 {
  USART2->SR &= ~USART_SR_TC;
 }
}

/* GPS port */
void USART3_IRQHandler(void)
{
 if ( USART3->SR & USART_SR_RXNE) //receive
 {
  char c = USART3->DR;
  USART3->SR &= ~USART_SR_RXNE;
 }
 else if ( USART3->SR & USART_SR_TC) //transfer
 {
  USART3->SR &= ~USART_SR_TC;
 }

}

void DMA1_Channel1_IRQHandler(void)
{
 if(DMA1->ISR & DMA_ISR_TCIF4)
 {
  DMA1->IFCR |= DMA_ISR_TCIF4;
 }
}



//TO USE: addr2line -e ./bin/program.elf -a 0x8002327 [GDB: p/x pc when it hit for(;;)]
USED void unwindCPUstack(word* stackAddress)
{
 /*
  These are volatile to try and prevent the compiler/linker optimising them
  away as the variables never actually get used.  If the debugger won't show the
  values of the variables, make them global my moving their declaration outside
  of this function.
  */
 volatile word r0 = stackAddress[0];
 volatile word r1 = stackAddress[1];
 volatile word r2 = stackAddress[2];
 volatile word r3 = stackAddress[3];
 volatile word r12 = stackAddress[4];
 /* Link register. */
 volatile word lr = stackAddress[5];
 /* Program counter. */
 volatile word pc = stackAddress[6];
 /* Program status register. */
 volatile word psr = stackAddress[7];


 SEGGER_RTT_printf(0, "CPU fatal error trapped\r\nSystem halted\r\n");
 SEGGER_RTT_printf(0, "*** CPU registers ***\r\n");
 SEGGER_RTT_printf(0, "R0:  0x%08X\nR1:  0x%08X\nR2:  0x%08X\nR3:  0x%08X\nR12: 0x%08X\n", r0, r1,
   r2, r3, r12);
 SEGGER_RTT_printf(0, "LR:  0x%08X\nPC:  0x%08X\nPSR: 0x%08X\n", lr, pc, psr);

 __enable_irq();
   __ISB();

 FATdisk d(1);
  FATFS filesystem;
  FIL gpx;
  FRESULT result = d.mount (&filesystem, "0:", 1);
  d.open(&gpx, "dump.txt", FA_CREATE_ALWAYS | FA_WRITE);
  d.close(&gpx);


 /* When the following line is hit, the variables contain the register values. */
 for (;;)
 {
		 delayus_asm(300000L);
		 BLINK;
 }
}

/* DTR */
void EXTI0_IRQHandler(void)
{
 //EXTI->PR = EXTI_PR_PR0;
}

void HardFault_Handler(void)
{
 __asm volatile
 (
   " tst lr, #4                                                \n"
   " ite eq                                                    \n"
   " mrseq r0, msp                                             \n"
   " mrsne r0, psp                                             \n"
   " ldr r1, [r0, #24]                                         \n"
   " ldr r2, handler_address_const                            \n"
   " bx r2                                                     \n"
   " handler_address_const: .word unwindCPUstack    \n"
 );
}

void MemManage_Handler(void)
{
 __asm volatile
 (
   " tst lr, #4                                                \n"
   " ite eq                                                    \n"
   " mrseq r0, msp                                             \n"
   " mrsne r0, psp                                             \n"
   " ldr r1, [r0, #24]                                         \n"
   " ldr r2, handler2_address_const                            \n"
   " bx r2                                                     \n"
   " handler2_address_const: .word unwindCPUstack    \n"
 );
}

void BusFault_Handler(void)
{
 __asm volatile
 (
   " tst lr, #4                                                \n"
   " ite eq                                                    \n"
   " mrseq r0, msp                                             \n"
   " mrsne r0, psp                                             \n"
   " ldr r1, [r0, #24]                                         \n"
   " ldr r2, handler3_address_const                            \n"
   " bx r2                                                     \n"
   " handler3_address_const: .word unwindCPUstack    \n"
 );
}

void UsageFault_Handler(void)
{
 __asm volatile
 (
   " tst lr, #4                                                \n"
   " ite eq                                                    \n"
   " mrseq r0, msp                                             \n"
   " mrsne r0, psp                                             \n"
   " ldr r1, [r0, #24]                                         \n"
   " ldr r2, handler4_address_const                            \n"
   " bx r2                                                     \n"
   " handler4_address_const: .word unwindCPUstack    \n"
 );
}
}
