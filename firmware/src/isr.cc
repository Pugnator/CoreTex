#include <global.hpp>

char gsmResponse[16];
char *gsmResponsePnt = gsmResponse;
bool gsmResponseRcvd = false;
static char gsmRcvdCtr = 0;

extern "C"
{	
	void SysTick_Handler(void)
	{	
		--tickcounter;		
		--uarttimeout;		
		--gsmtimeout;		
	}

	void SPI1_IRQHandler ( void )
	{
		if ( SET == (SPI1->SR & SPI_SR_RXNE) )
		{
					
			//spresponse = SPI1->DR;
		}
		else
		{
			PIN_LOW ( LED );
		}
		SPI1->SR&=~  SPI_SR_RXNE;
	}

	/* Camera/debug port */	

	void USART1_IRQHandler ( void )
	{
		if ( USART1->SR & USART_SR_RXNE ) //receive
		{
			char c = USART2->DR;
			USART1->DR = c;	
			USART1->SR&= ~USART_SR_RXNE;;
		}
		else if ( USART1->SR & USART_SR_TC ) //transfer
		{
			USART1->SR&= ~USART_SR_TC;
		}
		
	}

	/* GSM port */

	void USART2_IRQHandler ( void )
	{
		if ( USART2->SR & USART_SR_RXNE) //receive
		{		
			char c = USART2->DR;
			USART1->DR = c;			
			USART2->SR&= ~USART_SR_RXNE;
			if('\n' == c)
			{				
				gsmResponseRcvd = true;
				gsmRcvdCtr = 0;		
			}
			else if (!gsmResponseRcvd)
			{
				gsmRcvdCtr++;
				if(gsmRcvdCtr < sizeof gsmResponse)
				{
					*gsmResponsePnt++ = c;
				}				
			}			
		}
		else if ( USART2->SR & USART_SR_TC ) //transfer
		{
			USART2->SR&= ~USART_SR_TC;
		}
		
	}

	/* GPS port */
	void USART3_IRQHandler ( void )
	{
		if ( USART3->SR & USART_SR_RXNE ) //receive
		{
			char c = USART3->DR;
			USART1->DR = c;			
			USART3->SR&= ~USART_SR_RXNE;
		}
		else if ( USART3->SR & USART_SR_TC ) //transfer
		{
			USART3->SR&= ~USART_SR_TC;
		}
		
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
