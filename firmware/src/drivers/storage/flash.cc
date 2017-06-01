#include <drivers/storage/flash.hpp>

#ifdef FLASH_DEBUG
#define DEBUG_LOG SEGGER_RTT_printf
#else
#define DEBUG_LOG(...)
#endif

#define FLASH_KEY1      ((uint32_t)0x45670123)
#define FLASH_KEY2      ((uint32_t)0xCDEF89AB)

void Flash::mass_erase()
{
	FLASH->KEYR = FLASH_KEY1;
	FLASH->KEYR = FLASH_KEY2;

	while((FLASH->SR & FLASH_SR_BSY));
	FLASH->CR |= FLASH_CR_MER; //Mass Erase Set
	FLASH->CR |= FLASH_CR_STRT; //Start Page Erase
	while((FLASH->SR & FLASH_SR_BSY));
	FLASH->CR &= ~FLASH_CR_MER; //Mass Erase Clear
}
