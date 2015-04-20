#pragma once

namespace uart
{

class Stack
{

public:
	void flush ( void )
	{
		stackp = -1;
	};
	Stack() {};
	/**/
	char buf[64];
	bool ready;
	int stackp;
private:
	int push ( char c );
	int pop ( char* c );
	void print_stack ( void );
};

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
	void cls ( void );
	/**/
	//Stack data;
private:
	void send ( char c );
	char get ( void );
	void print ( char ch );
	void print ( int num );
	void print ( char const* str );
	void crlf ( void );
	void init ( int channel, int baud );
	/**/
	Stack data;
	char buf[8];
	int channel;
	USART_TypeDef* Reg;
};
}
