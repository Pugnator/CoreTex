#include <global.hpp>

extern "C" void SystemInit ( void )
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	PIN_OUT_PP ( LED );
}
