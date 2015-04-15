#include <global.hpp>

void USART1_IRQHandler ( void )
{
	PIN_TOGGLE ( LED );
}