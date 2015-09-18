#include <global.hpp>

/* UART handling class */

#define UARTirq (USART1_IRQn - 1)
uint32_t uarttimeout = 0;

using namespace uart;
Uart::Uart ( int ch, int bd, bool doinit )
{
	channel = ch;
	#ifdef __STM32__
	switch ( ch )
	{
		case 1:
			Reg = ( USART_TypeDef* ) USART1_BASE;
			break;
		case 2:
			Reg = ( USART_TypeDef* ) USART2_BASE;
			break;
		case 3:
			Reg = ( USART_TypeDef* ) USART3_BASE;
			break;
		default:
			assert ( 0 );
			;
	}
	if ( doinit )
	{
		init ( ch, bd );
	}
	#endif
}

void
Uart::disable ( void )
{
	#ifdef __STM32__
	switch ( channel )
	{
		case 1:
			NVIC_DisableIRQ ( USART1_IRQn );
			break;
		case 2:
			NVIC_DisableIRQ ( USART2_IRQn );
			break;
		case 3:
			NVIC_DisableIRQ ( USART3_IRQn );
			break;
	}
	#endif
}

void
Uart::send ( char ch )
{
	#ifdef __STM32__
	while ( ! ( Reg->SR & USART_SR_TC ) );
	#endif
	Reg->DR=ch;
}

char
Uart::get ( void )
{
	#ifdef __STM32__
	tickcounter = 1000;
	while ( ! ( Reg->SR & USART_SR_RXNE ) && tickcounter );
	#endif
	return Reg->DR;	
}

void
Uart::init ( int channel, int baud )
{
	#ifdef __STM32__
	switch ( channel )
	{
		case 1:
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
			PIN_OUT_ALT_PP ( TX1 );
			PIN_INPUT_FLOATING ( RX1 );
			break;
		case 2:
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
			PIN_OUT_ALT_PP ( TX2 );
			PIN_INPUT_FLOATING ( RX2 );
			break;
		case 3:
			RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
			PIN_OUT_ALT_PP ( TX3 );
			PIN_INPUT_FLOATING ( RX3 );
			break;
	}
	volatile int irqnum = UARTirq + channel;
	NVIC_EnableIRQ ( ( IRQn_Type ) irqnum );
	NVIC_SetPriority ( ( IRQn_Type ) irqnum, 3 );
	Reg->BRR = ( CRYSTAL + baud / 2 ) / baud;
	Reg->CR1 &= ~USART_CR1_M;
	Reg->CR2 &= ~USART_CR2_STOP;
	Reg->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	#endif
}

void
Uart::print ( char ch )
{
	send ( ch );
}

void
Uart::print ( char const* str )
{
	char const* p = str;
	while ( *p )
	{
		send ( *p++ );
	}
}

void
Uart::print ( int num )
{
	int temp = num;
	if ( num > INT_MAX || num < 0 )
	{
		print ( "Can't display number" );
		return;
	}
	
	int dcount = -1;
	/* Calculate number of digits */
	while ( 0!= temp )
	{
		temp /= 10;
		++ dcount;
	}
	
	temp = num;
	buf[dcount+1] = '\0';
	for ( int i = dcount; i>=0 ; i-- )
	{
		buf[i] = ( temp % 10 ) + '0';
		temp /= 10;
	}
	
	print ( buf );
}

void
Uart::crlf ( void )
{
	print ( "\r\n" );
}

void
Uart::recv ( char* c, int timeout )
{
	recvsize = 1;
}

/* Stack */

int Stack::push ( char c )
{
	if ( 0 == c )
		return 1;
		
	if ( stackp < STACK_DEPTH - 1 )
	{
		stack[++stackp] = c;
		return 0;
	}
	ready = true;
	return 1;
}

int Stack::pop ( char* c )
{
	if ( stackp >= 0 )
	{
		*c = stack[stackp--];
		return 0;
	}
	return 1;
}

void Stack::operator>> ( Uart port )
{
	for ( int i=1; i < stackp; i++ )
	{
		port < stack[i];
	}
	
}

char Stack::get ( void )
{
	char res;
	pop ( &res );
	return res;
}

void Stack::operator+ ( char c )
{
	push ( c );
}


char* Stack::str ( void )
{
	return stack ? stack : ( char* ) "";
}

