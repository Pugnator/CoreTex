#include <global.hpp>

typedef enum RMCFIELD
{
	NONE = 0, GPRMC , UTC, LAT, LATDIR, LONGT, LONGDIR, SPD, HEAD, DATE, MAGVAR, MAGVARDIR, MODE, END
}
RMCFIELD;

int field = NONE;
char fieldbuf[16] = {0};
char* fieldp = 0;

extern "C"
{

	void nmea ( char c )
	{
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
	
	void USART2_IRQHandler ( void )
	{
		if ( USART2->SR & USART_SR_RXNE ) //receive
		{
			char c = USART2->DR;
			//USART1->DR = c;// echo
			if ( '\n' == c )
			{				
				PIN_TOGGLE ( LED );				
			}
		}
		else if ( USART2->SR & USART_SR_TC ) //transfer
		{
			;
		}
		USART2->SR&=~ ( USART_SR_TC|USART_SR_RXNE );
	}
	
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
	
	extern int main(void);
	void Reset_Handler ( void )
	{
		__ASM volatile ( "cpsie i" : : : "memory" );
		main ();
		PROGRAM_END;
	}
}
