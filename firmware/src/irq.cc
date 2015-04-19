#include <global.hpp>

typedef enum RMCFIELD
{
	NONE = 0, GPRMC , UTC, LAT, LATDIR, LONGT, LONGDIR, SPD, HEAD, DATE, MAGVAR, MAGVARDIR, MODE, END
}
RMCFIELD;

int field = NONE;
char fieldbuf[16] = {0};
char* fieldp = 0;
static int stack_pointer;
char uart2stack[16];
bool stack_ok = false;

extern "C"
{
	static int
	push ( char c , char* stack )
	{
		if ( stack_pointer < 16 )
		{
			stack[++stack_pointer] = c;
			return 0;
		}
		return 1;
	}
	
	static int
	pop ( char* c , char* stack )
	{
		if ( stack_pointer >= 0 )
		{
			*c = stack[stack_pointer--];
			return 0;
		}
		return 1;
	}
	
	void nmea ( char c )
	{
		push ( c, uart2stack );
		if ( '\n' == c )
		{
			PIN_TOGGLE ( LED );
		}
		switch ( field )
		{
			case GPRMC:
				fieldp = fieldbuf;
				break;
			case UTC:
				*fieldp = c;
				break;
			case END:
				*fieldp=0;
				field = NONE;
				break;
		}
	}
	/* Debug */
	void USART1_IRQHandler ( void )
	{
		if ( USART1->SR & USART_SR_RXNE ) //receive
		{
			;
		}
		else if ( USART1->SR & USART_SR_TC ) //transfer
		{
			;
		}
		USART1->SR&=~ ( USART_SR_TC|USART_SR_RXNE );
	}
	
	/* GPS */
	void USART2_IRQHandler ( void )
	{
		if ( USART2->SR & USART_SR_RXNE ) //receive
		{
			char c = USART2->DR;
			USART1->DR = c;// echo
			nmea ( c );
		}
		else if ( USART2->SR & USART_SR_TC ) //transfer
		{
			;
		}
		USART2->SR&=~ ( USART_SR_TC|USART_SR_RXNE );
	}
	
	/* GSM */
	void USART3_IRQHandler ( void )
	{
		if ( USART3->SR & USART_SR_RXNE ) //receive
		{
		
		}
		else if ( USART3->SR & USART_SR_TC ) //transfer
		{
			;
		}
		USART3->SR&=~ ( USART_SR_TC|USART_SR_RXNE );
	}
	
	void RCC_IRQHandler ( void )
	{
	
	}
	
	void RTC_IRQHandler ( void )
	{
	
	}
	
	void SysTick_Handler ( void )
	{
	
	}
	
	void HardFault_Handler ( void )
	{
	
	}
}
