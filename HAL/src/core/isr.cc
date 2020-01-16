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
#include <core/gpio.hpp>
#include <core/spi.hpp>

volatile uint32_t tickcounter = 0;
volatile uint32_t timerms = 0;
volatile uint32_t uscounter = 0;

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
    Spi *i = (Spi *)HARDWARE_TABLE[SPI1_HANDLER];
    if (i)
    {
      i->isr(0);
      return;
    }
    if (SPI1->SR & SPI_SR_RXNE) //receive
    {
      short c = SPI1->DR;
      SPI1->SR &= ~SPI_SR_RXNE;
    }
    else if (SPI1->SR & SPI_SR_TXE) //transfer
    {
      SPI1->SR &= ~SPI_SR_TXE;
    }
  }

  void SPI2_IRQHandler(void)
  {
    if (SPI1->SR & SPI_SR_RXNE) //receive
    {
      short c = SPI1->DR;
      SPI1->SR &= ~SPI_SR_RXNE;
    }
    else if (SPI1->SR & SPI_SR_TXE) //transfer
    {
      SPI1->SR &= ~SPI_SR_TXE;
    }
  }

  void USART1_IRQHandler(void)
  {
    USART *i = (USART *)HARDWARE_TABLE[USART1_HANDLER];
    if (i)
    {
      i->isr(0);
      return;
    }

    if (USART1->SR & USART_SR_RXNE) //receive
    {
      short c = USART1->DR;
      USART1->SR &= ~USART_SR_RXNE;
      ;
    }
    else if (USART1->SR & USART_SR_TC) //transfer
    {
      USART1->SR &= ~USART_SR_TC;
    }
  }

  void USART2_IRQHandler(void)
  {
    USART *i = (USART *)HARDWARE_TABLE[USART2_HANDLER];
    if (i)
    {
      i->isr(0);
      return;
    }
    if (USART2->SR & USART_SR_RXNE) //receive
    {
      char c = USART2->DR;
      USART2->SR &= ~USART_SR_RXNE;
    }
    else if (USART2->SR & USART_SR_TC) //transfer
    {
      USART2->SR &= ~USART_SR_TC;
    }
  }

  void USART3_IRQHandler(void)
  {
    USART *i = (USART *)HARDWARE_TABLE[USART3_HANDLER];
    if (i)
    {
      i->isr(0);
      return;
    }
    if (USART3->SR & USART_SR_RXNE) //receive
    {
      char c = USART3->DR;
      USART3->SR &= ~USART_SR_RXNE;
    }
    else if (USART3->SR & USART_SR_TC) //transfer
    {
      USART3->SR &= ~USART_SR_TC;
    }
  }

  void RTC_IRQHandler(void){};
  void FLASH_IRQHandler(void){};
  void RCC_IRQHandler(void){};
  void EXTI0_IRQHandler(void)
  {
    IO::GPIO_pin *i = (IO::GPIO_pin *)HARDWARE_TABLE[EXTI0_HANDLER];
    if (i)
    {
      i->isr(0);
      return;
    }
  };
  void EXTI1_IRQHandler(void){};
  void EXTI2_IRQHandler(void){};
  void EXTI3_IRQHandler(void){};
  void EXTI4_IRQHandler(void){};
  void DMA1_Channel1_IRQHandler(void)
  {
    if (DMA1->ISR & DMA_ISR_TCIF4)
    {
      DMA1->IFCR |= DMA_ISR_TCIF4;
    }
  };
  void DMA1_Channel2_IRQHandler(void){};
  void DMA1_Channel3_IRQHandler(void){};
  void DMA1_Channel4_IRQHandler(void){};
  void DMA1_Channel5_IRQHandler(void){};
  void DMA1_Channel6_IRQHandler(void){};
  void DMA1_Channel7_IRQHandler(void){};
  void ADC1_IRQHandler(void)
  {
  }
  void ADC1_2_IRQHandler(void)
  {
    IO::GPIO_pin *i = (IO::GPIO_pin *)HARDWARE_TABLE[ADC1_2_HANDLER];
    if (i)
    {
      i->isr(0);
      return;
    }

    if (ADC1->SR & ADC_SR_EOC)
    {
      uint16_t result = ADC1->DR & 0xFFFF;
      ADC1->SR = 0;
    }
  }
  void USB_HP_CAN1_TX_IRQHandler(void){};
  void USB_LP_CAN1_RX0_IRQHandler(void){};
  void CAN1_RX1_IRQHandler(void){};
  void CAN1_SCE_IRQHandler(void){};
  void EXTI9_5_IRQHandler(void){};
  void TIM1_BRK_IRQHandler(void){};
  void TIM1_UP_IRQHandler(void){};
  void TIM1_TRG_COM_IRQHandler(void){};
  void TIM1_CC_IRQHandler(void){};

  void TIM2_IRQHandler(void)
  {
    if (TIM2->SR & TIM_SR_UIF)
    {      
      if(!++uscounter)
      {
        TIM2->CR1 &= ~(TIM_CR1_CEN);        
      }
      TIM2->SR &= ~(TIM_SR_UIF);
    }
  };

  void TIM3_IRQHandler(void){};
  void TIM4_IRQHandler(void){};

  //TO USE: addr2line -e ./bin/program.elf -a 0x8002327 [GDB: p/x pc when it hit for(;;)]
  USED void memory_dump(uint32_t *stackAddress)
  {
    Print("***Crash dump initiated***\n");
#ifdef __USE_MEMORY_DUMP
    /*
	 These are volatile to try and prevent the compiler/linker optimising them
	 away as the variables never actually get used.  If the debugger won't show the
	 values of the variables, make them global my moving their declaration outside
	 of this function.
	 */
    volatile uint32_t r0 = stackAddress[0];
    volatile uint32_t r1 = stackAddress[1];
    volatile uint32_t r2 = stackAddress[2];
    volatile uint32_t r3 = stackAddress[3];
    volatile uint32_t r12 = stackAddress[4];
    /* Link register. */
    volatile uint32_t lr = stackAddress[5];
    /* Program counter. */
    volatile uint32_t pc = stackAddress[6];
    /* Program status register. */
    volatile uint32_t psr = stackAddress[7];
    FATdisk d(1);
    FATFS filesystem;
    FIL dump;
    FRESULT result = d.mount(&filesystem, "0:", 1);

    PrintF("Dumping CPU registers...\r\n");

    d.open(&dump, "REGISTERS.TXT", FA_CREATE_ALWAYS | FA_WRITE);
    char registers[128];
    uint32_t written;
    xsprintf(
        registers,
        "R0:  0x%08X\nR1:  0x%08X\nR2:  0x%08X\nR3:  0x%08X\nR12: 0x%08X\r\n",
        r0, r1, r2, r3, r12);
    d.f_write(&dump, registers, strlen(registers), &written);
    xsprintf(registers, "LR:  0x%08X\nPC:  0x%08X\nPSR: 0x%08X\r\n", lr, pc,
             psr);
    d.f_write(&dump, registers, strlen(registers), &written);
    d.close(&dump);

    PrintF("Dumping RAM...\r\n");
    d.open(&dump, "RAM.DMP", FA_CREATE_ALWAYS | FA_WRITE);
    for (uint32_t a = 0x20000000U; a < 0x20000000 + 20480; a += 512)
    {
      result = d.f_write(&dump, *((uint32_t *)a), 512, &written);
      if (FR_OK != result)
      {
        PrintF("Failed to write the file...\r\n");
        break;
      }
      PrintF("%u is written\r\n", written);
    }
    d.close(&dump);

    PrintF("Dumping FLASH...\r\n");
    d.open(&dump, "FLASH.DMP", FA_CREATE_ALWAYS | FA_WRITE);
    for (uint32_t a = 0x08000000; a < 0x08000000 + 65536; a += 512)
    {
      result = d.f_write(&dump, *((uint32_t *)a), 512, &written);
      if (FR_OK != result)
      {
        PrintF("Failed to write the file...\r\n");
        break;
      }
      PrintF("%u is written\r\n", written);
    }
    d.close(&dump);
#endif
    for (;;)
      ;
  }

  void HardFault_Handler(void)
  {
    __asm volatile(
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler_address_const                            \n"
        " bx r2                                                     \n"
        " handler_address_const: .word memory_dump    \n");
  }

  void MemManage_Handler(void)
  {
    __asm volatile(
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler2_address_const                            \n"
        " bx r2                                                     \n"
        " handler2_address_const: .word memory_dump    \n");
  }

  void BusFault_Handler(void)
  {
    __asm volatile(
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler3_address_const                            \n"
        " bx r2                                                     \n"
        " handler3_address_const: .word memory_dump    \n");
  }

  void UsageFault_Handler(void)
  {
    __asm volatile(
        " tst lr, #4                                                \n"
        " ite eq                                                    \n"
        " mrseq r0, msp                                             \n"
        " mrsne r0, psp                                             \n"
        " ldr r1, [r0, #24]                                         \n"
        " ldr r2, handler4_address_const                            \n"
        " bx r2                                                     \n"
        " handler4_address_const: .word memory_dump    \n");
  }
}
