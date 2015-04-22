#include <global.hpp>

extern "C"
{
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
			if('\n' == c)
			{
				PIN_TOGGLE(LED);
			}
			usart2data + c;			
			//USART1->DR = c;// echo			
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
}
