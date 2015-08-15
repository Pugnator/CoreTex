#include <global.hpp>

extern "C" void SystemInit ( void )
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	/* System timer config */
	SysTick->LOAD=TIMERTICK;
 	SysTick->VAL=TIMERTICK;
 
 	SysTick->CTRL =	SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	PIN_OUT_PP ( LED );
	PIN_OUT_PP ( SWEEP );
	PIN_INPUT_PU ( GSMDCD );
	PIN_OUT_PP ( GSMCTS );
	PIN_OUT_PP ( GSMDTR );
}
