#pragma once

#include <global.hpp>
#include <common.hpp>
#include <core/stm32f10x.hpp>

#define FLASH_KEY1      ((uint32_t)0x45670123)
#define FLASH_KEY2      ((uint32_t)0xCDEF89AB)
#define EEPROM_DATA (FLASH_BASE+(63*1024))

class Eeprom
{
public:
  Eeprom()
  {
    erase();
  }
  void cell_write(uint16_t cell, uint8_t value);
  uint8_t cell_read(uint16_t cell);
private:
  void erase();
  void unlock();
  void lock();
};
