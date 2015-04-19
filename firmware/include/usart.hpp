#pragma once

namespace uart
{
class Uart
{
public:
	Uart ( int ch, int bd, bool doinit );
	void disable ( void );
	void operator= ( char const* str );
	void operator= ( char c );
	void operator= ( int num );
	void operator+= ( char const* str );
	void operator+= ( char c );
	void operator+= ( int num );
	void operator& ( char const* str );
	void print_stack ( void );
	int push ( char c );
	int pop ( char* c );
private:
	void send ( char c );
	char get ( void );
	void print ( char ch );
	void print ( int num );
	void print ( char const* str );
	void crlf ( void );
	void init ( void );
	int channel;
	int baud;
	char buf[64];
	int stack_pointer;
	char stack[16];
	USART_TypeDef* Reg;
};
}
