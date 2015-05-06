#include <global.hpp>

uint16_t spresponse = 0;

extern "C"
{
	void SPI1_IRQHandler ( void )
	{
		if ( SET == (SPI1->SR & SPI_SR_RXNE) )
		{
			PIN_TOGGLE ( LED );			
			spresponse = SPI1->DR;
		}
		else
		{
			PIN_LOW ( LED );
		}
		SPI1->SR&=~  SPI_SR_RXNE;
	}
	
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
		if ( USART2->SR & USART_SR_RXNE && !usart2data.ready ) //receive
		{
			char c = USART2->DR;
			parseNMEA ( c );
			if ( '$' != c && -1 == usart2data.len() )
			{
				USART2->SR&=~ ( USART_SR_TC|USART_SR_RXNE );
				return;
			}

			if ( '\n' == c )
			{
				usart2data + c;
				usart2data.ready = true;
			}
			else
			{
				usart2data + c;
			}

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

	void HardFault_Handler(void)
	{
	  for( ;; );
	}

	void MemManage_Handler(void)
	{
	  for( ;; );
	}

	void BusFault_Handler(void)
	{
	 for( ;; );
	}

	void UsageFault_Handler(void)
	{
	 for( ;; );
	}
}
