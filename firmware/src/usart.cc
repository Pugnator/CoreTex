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
		this->stm32_init ( );
	}
}

Uart::~Uart ( void )
{
	;
}

void Uart::stm32_sendchr ( char ch )
{
	while ( ! ( Reg->SR & USART_SR_TC ) );
	Reg->DR=ch;
}

char Uart::stm32_getchr ( void )
{
	while ( ! ( Reg->SR & USART_SR_RXNE ) );
	return Reg->DR;
}

void Uart::stm32_init ( void )
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	Reg->BRR = ( CRYSTAL+baud/2 ) / baud;
	Reg->CR1 &= ~USART_CR1_M;
	Reg->CR2 &= ~USART_CR2_STOP;
	Reg->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
	switch ( channel )
	{
		case 1:
			PIN_OUT_ALT_PP ( TX1 );
			PIN_INPUT_FLOATING ( RX1 );
			break;
		case 2:
			PIN_OUT_ALT_PP ( TX2 );
			PIN_INPUT_FLOATING ( RX2 );
			break;
		case 3:
			break;
	}
}

void Uart::print ( char ch )
{
	this->stm32_sendchr ( ch );
}

void Uart::print ( char const* str )
{
	char const* p = str;
	while ( *p )
	{
		this->stm32_sendchr ( *p++ );
	}
}

void Uart::print ( int num )
{
	if ( num > 9999 )
	{
		this->print ( NUMBER_TOO_LARGE );
		return;
	}
	int out = num;
	buf[4] = '\0';
	buf[3] = ( out % 10 ) + '0';
	out /= 10;
	buf[2] = ( out % 10 ) + '0';
	out /= 10;
	buf[1] = ( out % 10 ) + '0';
	out /= 10;
	buf[0] = ( out % 10 ) + '0';
	this->print ( buf );
}
