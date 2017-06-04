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

void Eeprom::write(uint16_t cell, uint8_t value)
{
  if (EEPROM_SIZE * 1024 <= cell)
  {
    DEBUG_LOG(0, "requested EEPROM cell is out of bounds [%u > %u]\r\n", cell, EEPROM_SIZE * 1024);
    return;
  }
  DEBUG_LOG(0, "Writing to the FLASH, page address: 0x%X [*cell = 0x%X]\r\n", EEPROM_DATA, *((uint16_t*)EEPROM_DATA + cell));
  uint8_t *backup = reinterpret_cast<uint8_t*> (ALLOC(1024));
  if(!backup)
  {
    DEBUG_LOG(0, "Failed to allocate 1024 bytes\r\n");
    return;
  }
  memcpy(backup, reinterpret_cast<void*>(EEPROM_DATA), 1024);
  backup[cell] = value;
  DEBUG_LOG(0, "Memory page content: 0x%X\r\n", backup[cell]);
  unlock();
  erase();
  FLASH->CR |= FLASH_CR_PG;
  while((FLASH->SR & FLASH_SR_BSY));
  for(uint16_t i = 0; i < 1024; i += 2)
  {
    *(reinterpret_cast<volatile uint16_t*>(EEPROM_DATA + i)) = (backup[i+1] << 8) | (backup[i] & 0xff);
    while(!(FLASH->SR & FLASH_SR_EOP));
    FLASH->SR = FLASH_SR_EOP;
  }
  FLASH->CR &= ~FLASH_CR_PG;
  lock();
  FREE(backup);
  DEBUG_LOG(0, "Result: 0x%X\r\n", *((uint8_t*)EEPROM_DATA+cell));
}

uint8_t Eeprom::read(uint16_t cell)
{
  if (EEPROM_SIZE * 1024 <= cell)
  {
    DEBUG_LOG(0, "requested EEPROM cell is out of bounds [%u > %u]\r\n", cell, EEPROM_SIZE * 1024);
    return 0;
  }
  return *(reinterpret_cast<volatile uint8_t*>(EEPROM_DATA + cell));
}

void Eeprom::erase()
{
  DEBUG_LOG(0, "Erasing flash page 0x%X\r\n", reinterpret_cast<word>(EEPROM_DATA));
  unlock();
  while (FLASH->SR & FLASH_SR_BSY);
  if (FLASH->SR & FLASH_SR_EOP)
  {
    FLASH->SR = FLASH_SR_EOP;
  }
  FLASH->CR |= FLASH_CR_PER; //Page Erase Set
  FLASH->AR =  reinterpret_cast<word>(EEPROM_DATA);//Page Address
  FLASH->CR |= FLASH_CR_STRT; //Start Page Erase
  while(!(FLASH->SR & FLASH_SR_EOP));
  FLASH->SR = FLASH_SR_EOP;
  FLASH->CR &= ~FLASH_CR_PER; //Page Erase Clear
}

void Eeprom::unlock()
{
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
}

void Eeprom::lock()
{
  FLASH->CR |= FLASH_CR_LOCK;
}
