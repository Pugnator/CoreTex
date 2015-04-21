#include <global.hpp>

char stack[STACK_DEPTH];
int stackp = -1;
bool nmeaready= false;;
extern "C"
{

	int push ( char c )
	{
		if ( stackp < STACK_DEPTH - 1 )
		{
			stack[++stackp] = c;
			return 0;
		}
		return 1;
	}
	
	int pop ( char* c )
	{
		if ( stackp >= 0 )
		{
			*c = stack[stackp--];
			return 0;
		}
		return 1;
	}
	
	void print_stack ( void )
	{
	
		for ( int i=0; i <= stackp; i++ )
		{
			;
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
			usart2data + c;			
			//USART1->DR = c;// echo
			if ( '\n' != c && ! nmeaready )
			{
				push ( c );
			}
			else if ( '\n' == c )
			{
				nmeaready = true;
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
}
