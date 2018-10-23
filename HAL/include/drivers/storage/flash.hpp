#pragma once

#include <global.hpp>
#include <common.hpp>
#include <core/stm32f10x.hpp>

class Flash
{
public:
	void mass_erase(void);
	void erase_page(uint32_t page);
	void write_page(uint32_t page, uint8_t *buffer);
	void write_buffer(uint32_t page, uint8_t *buffer, uint32_t size, uint32_t offset);
};
