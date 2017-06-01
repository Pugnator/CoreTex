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
#include <common.hpp>
#include <log.hpp>
#include <core/vmmu.hpp>
#include <core/io_macro.hpp>
#include <core/isr_helper.hpp>
#include <core/stm32f10x.hpp>
#include <global.hpp>
#include <drivers/storage/eeprom.hpp>

#ifdef EEPROM_DEBUG
#define DEBUG_LOG SEGGER_RTT_printf
#else
#define DEBUG_LOG(...)
#endif

const uint16_t __attribute__((section (".eepromSection"))) EEPROM_DATA[512] = { 0xAAAA };

void Eeprom::ewrite(word cell, uint16_t value)
{
  unlock();
  cell_erase((word)EEPROM_DATA);
  uint16_t *backup = reinterpret_cast<uint16_t*> (stalloc(1024));
  FLASH->CR |= FLASH_CR_PG;
  while((FLASH->SR&FLASH_SR_BSY));
  *(const_cast<uint16_t*>(EEPROM_DATA)) = 0xAAAA;
  FLASH->CR &= ~FLASH_CR_PG;
}

uint16_t Eeprom::eread(word cell)
{
  return 0;
}

void Eeprom::cell_erase(word cell)
{
  DEBUG_LOG(0, "Writing to the FLASH, page address: 0x%X [*page = 0x%X]\r\n", cell, *((word*)cell));
  unlock();
  while((FLASH->SR&FLASH_SR_BSY));
  FLASH->CR |= FLASH_CR_PER; //Page Erase Set
  FLASH->AR = cell;//Page Address
  FLASH->CR |= FLASH_CR_STRT; //Start Page Erase
  while((FLASH->SR&FLASH_SR_BSY));
  FLASH->CR &= ~FLASH_CR_PER; //Page Erase Clear
}

void Eeprom::unlock()
{
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}
