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
#include <core/vmmu.hpp>
#include <global.hpp>
#include <setjmp.h>

jmp_buf ex_buf__;

void enable_pll()
{
  /* Enable FLASH preselect */
  FLASH->ACR |= FLASH_ACR_PRFTBE;

  /* Setup FLASH latency, 2 wait cycles*/

  FLASH->ACR &= static_cast<uint32_t>((static_cast<uint32_t>(~FLASH_ACR_LATENCY)));
  FLASH->ACR |= static_cast<uint32_t>(FLASH_ACR_LATENCY_2);

  /* HCLK = SYSCLK */
  RCC->CFGR |= static_cast<uint32_t>(RCC_CFGR_HPRE_DIV1);

  /* PCLK2 = HCLK */
  RCC->CFGR |= static_cast<uint32_t>(RCC_CFGR_PPRE2_DIV1);

  /* PCLK1 = HCLK */
  RCC->CFGR |= static_cast<uint32_t>(RCC_CFGR_PPRE1_DIV2);

  /* PLLCLK = HSE * 9 = 72 MHz 
  For 8MHz crystal*/

  RCC->CFGR &= static_cast<uint32_t>((static_cast<uint32_t>(~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL))));
  RCC->CFGR |= static_cast<uint32_t>((RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9));

  /* Включаем PLL */
  RCC->CR |= RCC_CR_PLLON;

  while (!RCC->CR & RCC_CR_PLLRDY)
    ;

  RCC->CFGR &= static_cast<uint32_t>((static_cast<uint32_t>(~(RCC_CFGR_SW))));
  RCC->CFGR |= static_cast<uint32_t>(RCC_CFGR_SW_PLL);

  while ((RCC->CFGR & static_cast<uint32_t>(RCC_CFGR_SWS)) != 0x08)
    ;
}

extern "C" void SystemInit(void)
{
  vmmu_init();  
  //remap_vector_table();

  RCC->CR |= RCC_CR_HSEON;
  uint32_t timeout = ~0;

  while ((!RCC->CR & RCC_CR_HSERDY) && --timeout)
    ;

  if (timeout)
  {
    enable_pll();
  }
  else
  {
    //XXX: error out?
  }

  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;

  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

  /* System timer config */
  SysTick->LOAD = TIMERTICK;
  SysTick->VAL = TIMERTICK;

  SCB->CCR |= 0x18;         // enable div-by-0 and unaligned fault
  SCB->SHCSR |= 0x00070000; // enable Usage Fault, Bus Fault, and MMU Fault

  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}
