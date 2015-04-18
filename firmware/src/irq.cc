#include <global.hpp>

extern "C"
{
	typedef enum RMCFIELD
	{
		NONE = 0, GPRMC , UTC, LAT, LATDIR, LONGT, LONGDIR, SPD, HEAD, DATE, MAGVAR, MAGVARDIR, MODE, END
	}
	RMCFIELD;
	
	int field = NONE;
	char fieldbuf[16] = {0};
	char* fieldp = 0;
	
	void nmea ( char c )
	{
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
			//USART1->DR = c;// echo
			switch ( c )
			{
				case '$':
				case '\r':
					break;
				case '\n':
					field++;
					*fieldp = 0;
					PIN_TOGGLE(LED);
					break;
				case ',':
					field++;
					fieldp = fieldbuf;
					*fieldp = 0;
					break;
				default:
					;nmea ( c );
			}
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
}
