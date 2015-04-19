#include <global.hpp>

/* UART handling class */

using namespace uart;
Uart::Uart ( int ch, int bd, bool doinit )
{
	channel = ch;
	baud = bd;
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
		this->init ( );
	}
}

void Uart::disable ( void )
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

void Uart::send ( char ch )
{
	while ( ! ( Reg->SR & USART_SR_TC ) );
	Reg->DR=ch;
}

char Uart::get ( void )
{
	while ( ! ( Reg->SR & USART_SR_RXNE ) );
	return Reg->DR;
}

void Uart::init ( void )
{
	switch ( channel )
	{
		case 1:
			RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
			PIN_OUT_ALT_PP ( TX1 );
			PIN_INPUT_FLOATING ( RX1 );
			NVIC_EnableIRQ ( USART1_IRQn );
			NVIC_SetPriority ( USART1_IRQn, 3 );
			break;
		case 2:
			RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
			PIN_OUT_ALT_PP ( TX2 );
			PIN_INPUT_FLOATING ( RX2 );
			NVIC_EnableIRQ ( USART2_IRQn );
			NVIC_SetPriority ( USART2_IRQn, 3 );
			break;
		case 3:
			break;
	}
	Reg->BRR = ( CRYSTAL + baud / 2 ) / baud;
	Reg->CR1 &= ~USART_CR1_M;
	Reg->CR2 &= ~USART_CR2_STOP;
	Reg->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}

void Uart::print ( char ch )
{
	this->send ( ch );
}

void Uart::print ( char const* str )
{
	char const* p = str;
	while ( *p )
	{
		this->send ( *p++ );
	}
}

void Uart::print ( int num )
{		
	int temp = num;
	if ( num > INT_MAX || num < 0 )
	{		
		return;
	}		
	
	int dcount = -1;
	/* Calculate number of digits */
	while (0!= temp )
	{
		temp /= 10;
		++ dcount;
	}	
	
	temp = num;
	buf[dcount+1] = '\0';
	for(int i = dcount; i>=0 ; i--)
	{
		buf[i] = ( temp % 10 ) + '0';
		temp /= 10;	
	}
	
	this->print ( buf );
}

void Uart::crlf ( void)
{
	this->print ( "\r\n" );
}

void Uart::operator+=(char const *str) 
{
    this->print ( str );
}

void Uart::operator+=(char c) 
{
    this->print ( c );
}

void Uart::operator+=(int num) 
{
    this->print ( num );
}

void Uart::operator=(char const *str) 
{
    this->print ( str );
    this->crlf();
}

void Uart::operator=(char c) 
{
    this->print ( c );
    this->crlf();
}

void Uart::operator=(int num) 
{
    this->print ( num );
    this->crlf();
}

void Uart::operator&(char const *str) 
{
    strcpy(buf, str);
}
