#include <global.h>

#define RX1 A, 10, SPEED_50MHz
#define TX1 A, 9, SPEED_50MHz
#define RX2 A, 3, SPEED_50MHz
#define TX2 A, 2, SPEED_50MHz

struct 
{
	int baud;
}uart_ctx;

void uart1_putc(int c)
{
	while(!(USART1->SR & USART_SR_TC));
	USART1->DR=c;
}

int uart1_getc(void)
{
	while(!(USART1->SR & USART_SR_RXNE));		
	return USART1->DR;	
}

void uart2_putc(int c)
{
	while(!(USART2->SR & USART_SR_TC));
	USART2->DR=c;
}

int uart2_getc(void)
{
	while(!(USART2->SR & USART_SR_RXNE));		
	return USART2->DR;	
}

void uprint_str( char *text)
{
	char *p = text;
	while (*p)
	{
		uart1_putc (*p);
		p++;
	}
}

void usart1_init(int baud)
{	
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	PIN_OUT_ALT_PP( TX1 );
	PIN_INPUT_FLOATING( RX1 );
	USART1->BRR = (CRYSTAL+baud/2) / baud;
	USART1->CR1 &= ~USART_CR1_M;
  	USART1->CR2 &= ~USART_CR2_STOP;
	USART1->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}

void usart2_init(int baud)
{	
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	PIN_OUT_ALT_PP( TX2 );
	PIN_INPUT_FLOATING( RX2 );
	USART2->BRR = (CRYSTAL+baud/2) / baud;
	USART2->CR1 &= ~USART_CR1_M;
  	USART2->CR2 &= ~USART_CR2_STOP;
	USART2->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;
}
