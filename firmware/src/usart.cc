#include <global.hpp>

/* UART handling class */

#define UARTirq (USART1_IRQn - 1)

using namespace uart;
Uart::Uart ( int ch, int bd, bool doinit )
{	
	channel = ch;
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
		this->init ( ch, bd );
	}	
}

void
Uart::disable ( void )
{
	switch ( channel )
	{
		case 1:
			NVIC_DisableIRQ ( USART1_IRQn );
			break;
		case 2:
			NVIC_DisableIRQ ( USART2_IRQn );
			break;
	}
}

void
Uart::send ( char ch )
{
	while ( ! ( Reg->SR & USART_SR_TC ) );
	Reg->DR=ch;
}

char
Uart::get ( void )
{
	while ( ! ( Reg->SR & USART_SR_RXNE ) );
	return Reg->DR;
}

void
Uart::init ( int channel, int baud )
{
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
			break;
	}
	volatile int irqnum = UARTirq + channel;
	NVIC_EnableIRQ ( ( IRQn_Type ) irqnum );
	NVIC_SetPriority ( ( IRQn_Type ) irqnum, 3 );
	Reg->BRR = ( CRYSTAL + baud / 2 ) / baud;
	Reg->CR1 &= ~USART_CR1_M;
	Reg->CR2 &= ~USART_CR2_STOP;
	Reg->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}

void
Uart::print ( char ch )
{
	this->send ( ch );
}

void
Uart::print ( char const* str )
{
	char const* p = str;
	while ( *p )
	{
		this->send ( *p++ );
	}
}

void
Uart::print ( int num )
{
	int temp = num;
	if ( num > INT_MAX || num < 0 )
	{
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
	
	this->print ( buf );
}

void
Uart::crlf ( void )
{
	this->print ( "\r\n" );
}

void
Uart::recv ( char* c, int timeout)
{
	recvsize = 1;	
}

/* Clear terminal ESC[2J */
void
Uart::cls ( void )
{
	this->print ( ( char ) 27 );
	this->print ( "[2J" );
	this->print ( ( char ) 27 );
	this->print ( "[H" );
}

/* Stack */

int Stack::push ( char c )
{
	if ( stackp < STACK_DEPTH - 1 )
	{
		stack[++stackp] = c;
		return 0;
	}
	full = true;
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
	for ( int i=0; i <= stackp; i++ )
	{
		port < stack[i];
	}
	
}

char Stack::get(void)
{
	char res;
	this->pop(&res);
	return res;
}

void Stack::operator+ ( char c)
{
	this->push(c);
}
