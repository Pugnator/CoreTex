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
	Uart ( int ch, int bd );
	~Uart ( void );
	void ( Uart:: *init ) ();
	void ( Uart:: *sendchr ) ( char );
	char ( Uart:: *getchr ) ();
private:
	/* Hardware dependend functions */
	void stm32_init1 ( void );
	void stm32_init2 ( void );
	void stm32_sendchr1 ( char c );
	char stm32_getchr1 ( void );
	void stm32_sendchr2 ( char c );
	char stm32_getchr2 ( void );
	int channel;
	int baud;
	char rxbuf[8];
	char txbuf[8];
	int ch_number;
};
}