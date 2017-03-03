#include <global.hpp>
#include <common.hpp>
#include <core/stm32f10x.hpp>
#include "../generic/iodriver.hpp"

namespace NAND
{
class Flash : public IODriver
{
public:
	void mass_erase(void);
	void erase_page(word page);
	void write_page(word page, uint8_t *buffer);
	void write_buffer(word page, uint8_t *buffer, word size, word offset);
};
}
