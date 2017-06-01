#pragma once

#include <global.hpp>
#include <common.hpp>
#include <core/stm32f10x.hpp>

#define FLASH_KEY1      ((uint32_t)0x45670123)
#define FLASH_KEY2      ((uint32_t)0xCDEF89AB)

extern const uint16_t __attribute__((section (".eepromSection"))) EEPROM_DATA[512];

class Eeprom
{
public:
  Eeprom()
  {
    cell_erase((word)EEPROM_DATA);
  }
  void ewrite(word cell, uint16_t value);
  uint16_t eread(word cell);
private:
  void cell_erase(word cell);
  void unlock();
};
