#include <global.hpp>

/*void uprint_str ( char* text )
{
	char* p = text;
	while ( *p )
	{
		uart1_putc ( *p );
		p++;
	}
}*/

/* UART handling class */

using namespace uart;
Uart::Uart ( int ch, int bd )
{
	channel = ch;
	baud = bd;
	switch ( ch )
	{
		case 1:
		#ifdef __arm__
			init = &Uart::stm32_init1;
			sendchr = &Uart::stm32_sendchr1;
			getchr = &Uart::stm32_getchr1;
		#elif _WIN32
			//TODO
		#elif __linux
			//TODO
		#endif
			break;

		case 2:
		#ifdef __arm__
			init = &Uart::stm32_init2;
			sendchr = &Uart::stm32_sendchr2;
			getchr = &Uart::stm32_getchr2;
		#elif _WIN32
			//TODO
		#elif __linux
			//TODO
		#endif
			break;
		default:
			//TODO: Check maximum channel number on current system
			;
	}
	memset ( rxbuf, 0, sizeof  rxbuf );
	memset ( txbuf, 0, sizeof  txbuf );
}

Uart::~Uart ( void )
{
	;
}

void Uart::stm32_sendchr1 ( char ch )
{
	while ( ! ( USART1->SR & USART_SR_TC ) );
	USART1->DR=ch;
}

char Uart::stm32_getchr1 ( void )
{
	while ( ! ( USART1->SR & USART_SR_RXNE ) );
	return USART1->DR;
}

void Uart::stm32_sendchr2 ( char ch )
{
	while ( ! ( USART2->SR & USART_SR_TC ) );
	USART2->DR=ch;
}

char Uart::stm32_getchr2 ( void )
{
	while ( ! ( USART2->SR & USART_SR_RXNE ) );
	return USART2->DR;
}

void Uart::stm32_init1 ( void )
{
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	PIN_OUT_ALT_PP ( TX1 );
	PIN_INPUT_FLOATING ( RX1 );
	USART1->BRR = ( CRYSTAL+baud/2 ) / baud;
	USART1->CR1 &= ~USART_CR1_M;
	USART1->CR2 &= ~USART_CR2_STOP;
	USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}

void Uart::stm32_init2 ( void )
{
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	PIN_OUT_ALT_PP ( TX2 );
	PIN_INPUT_FLOATING ( RX2 );
	USART2->BRR = ( CRYSTAL+baud/2 ) / baud;
	USART2->CR1 &= ~USART_CR1_M;
	USART2->CR2 &= ~USART_CR2_STOP;
	USART2->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}