#pragma once

namespace uart
{
class Uart
{
public:
	Uart ( int ch, int bd = 9600, bool doinit = false );
	void disable ( void );
	void operator= ( char const* str );
	void operator= ( char c );
	void operator= ( int num );
	void operator+= ( char const* str );
	void operator+= ( char c );
	void operator+= ( int num );
	void operator& ( char const* str );
	void print_stack ( void );
	void recv ( char* c, int timeout = 0 );
	void recv ( char* str, int len = 0, int timeout = 0 );
	void irq ( void );
private:
	void send ( char c );
	char get ( void );
	void print ( char ch );
	void print ( int num );
	void print ( char const* str );
	void crlf ( void );
	void init ( int channel, int baud );
	char buf[64];
	int channel;
	USART_TypeDef* Reg;
};
}
