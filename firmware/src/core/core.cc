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
#include <core/io_macro.hpp>
#include <core/isr_helper.hpp>
#include <core/vmmu.hpp>
#include <global.hpp>

extern "C" void SystemInit(void)
{
 vmmu_init();
 remap_vector_table();

 RCC->CR |= 0x00000001;
 /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
 RCC->CFGR &= 0xF0FF0000;
 /* Reset HSEON, CSSON and PLLON bits */
 RCC->CR &= 0xFEF6FFFF;
 /* Reset HSEBYP bit */
 RCC->CR &= 0xFFFBFFFF;
 /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
 RCC->CFGR &= 0xFF80FFFF;
 /* Disable all interrupts and clear pending bits  */
 RCC->CIR = 0x009F0000;

 RCC->APB2ENR |=
   RCC_APB2ENR_IOPAEN |
   RCC_APB2ENR_IOPBEN |
   RCC_APB2ENR_IOPCEN |
   RCC_APB2ENR_AFIOEN;

 /* System timer config */
 SysTick->LOAD = TIMERTICK;
 SysTick->VAL = TIMERTICK;

 SCB->CCR |= 0x18; // enable div-by-0 and unaligned fault
 SCB->SHCSR |= 0x00070000; // enable Usage Fault, Bus Fault, and MMU Fault

 SysTick->CTRL =
   SysTick_CTRL_CLKSOURCE_Msk |
   SysTick_CTRL_TICKINT_Msk |
   SysTick_CTRL_ENABLE_Msk;

 /*  AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0_PA;
     AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;
     EXTI->RTSR |= EXTI_RTSR_TR0;
     EXTI->FTSR |= EXTI_FTSR_TR0;
     EXTI->IMR |= EXTI_IMR_MR0;
     NVIC_EnableIRQ ( ( IRQn_Type ) EXTI0_IRQn );
     NVIC_SetPriority ( ( IRQn_Type ) EXTI0_IRQn, 15 );*/
 PIN_OUT_PP(LED);
 PIN_LOW(LED);
}
