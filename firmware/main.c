#include <global.h>
#define LED C, 13, SPEED_50MHz

void USART1_IRQHandler (void)
{
	PIN_TOGGLE(LED);
}


int main( void )
{		
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	PIN_OUT_PP(LED);	
	usart1_init(9600);
	usart2_init(115200);	
	__ASM volatile ("cpsie i" : : : "memory");
	uprint_str("\r\nMCU started\r\n");	
	
	 
	 
	PROGRAM_END;
}
