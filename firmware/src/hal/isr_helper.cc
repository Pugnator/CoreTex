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
#include <hal/stm32f10x.hpp>
#include <global.hpp>
#include <string.h>

volatile word __attribute__((section (".vectorsSection")))
IRQ_VECTOR_TABLE[76] =
  { 0 };

inline bool is_in_interrupt()
{
    return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0 ;
}

void
remap_vector_table (void)
{
  //VTOR is 0 on startup, so we change VTOR only once
  if (SCB->VTOR)
    return;
  memcpy ((void*) IRQ_VECTOR_TABLE, (void*) SCB->VTOR, sizeof IRQ_VECTOR_TABLE);
  __disable_irq();
  SCB->VTOR = (uint32_t) (IRQ_VECTOR_TABLE); //Set VTOR offset
  __DSB (); //Complete all memory requests
  __enable_irq();
  __ISB();
}
