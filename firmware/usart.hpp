#pragma once
extern char usart1_fifo[8];
extern char usart1_fifo_len;

void uart1_putc ( int c );
void uart2_putc ( int c );
int uart1_getc ( void );
int uart2_getc ( void );
void uprint_str ( char* text );

namespace uart
{
class Uart
{
public:
	Uart ( int channel, int baud );
	~Uart ( void );
	void init ( void );
private:
	int channel;
	int baud;
	char rxbuf[8];
	char txbuf[8];
	int ch_number;
};
}