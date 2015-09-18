#include <global.hpp>

extern "C" void SystemInit ( void )
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN;
	/* AF clock enable */
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	/* System timer config */
	SysTick->LOAD=TIMERTICK;
	SysTick->VAL=TIMERTICK;
	
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
	PIN_OUT_PP ( LED );
	PIN_OUT_PP ( SWEEP );
	PIN_INPUT_FLOATING ( GSMDCD );
	PIN_INPUT_FLOATING ( GSMCTS );
	PIN_OUT_PP ( GSMDTR );
	
	/*  AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0_PA;
	    AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;
	    EXTI->RTSR |= EXTI_RTSR_TR0;
	    EXTI->FTSR |= EXTI_FTSR_TR0;
	    EXTI->IMR |= EXTI_IMR_MR0;
	    NVIC_EnableIRQ ( ( IRQn_Type ) EXTI0_IRQn );
	    NVIC_SetPriority ( ( IRQn_Type ) EXTI0_IRQn, 15 );*/
}
