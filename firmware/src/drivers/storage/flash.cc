#include <global.hpp>
#include <common.hpp>
#include <hal/stm32f10x.hpp>

#define FLASH_KEY1      ((uint32_t)0x45670123)
#define FLASH_KEY2      ((uint32_t)0xCDEF89AB)


void mass_erase()
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;

	while((FLASH->SR & FLASH_SR_BSY));
	FLASH->CR |= FLASH_CR_MER; //Mass Erase Set
	FLASH->CR |= FLASH_CR_STRT; //Start Page Erase
	while((FLASH->SR & FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_CR_MER; //Mass Erase Clear

}
