#include <global.hpp>

using namespace mcu;

void delay_ms (int ms)
{
	uint64_t nCount=(CRYSTAL/10000/2)*ms;
    for (; nCount!=0; nCount--);					    
}

void delay (int s)
{
	delay_ms(s*1000);				    
}

Mcu::Mcu()
{

}
