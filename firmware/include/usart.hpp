#pragma once

#define STACK_DEPTH 64

namespace uart
{

class Uart;

class Stack
{

public:	
	Stack(void) {stackp = -1; full = false;};		
	void operator+ ( char c );		
	char get();	
	void reset(void){stackp = -1;};
	char operator--();	
	void operator>> ( Uart port );
	bool full;
private:	
	char stack[STACK_DEPTH];	
	int stackp;
	int push ( char c );
	int pop ( char* c );	
	
};

class Uart
{

public:
	Uart ( int ch, int bd = 9600, bool doinit = false );
	void disable ( void );
	void operator>> ( char const* str );
	void operator>> ( char c );
	void operator>> ( int num );
	void operator> ( char const* str );
	void operator> ( char c );
	void operator> ( int num );		
	void recv ( char* c, int timeout = 0 );	
	void cls ( void );	
	Stack data;
private:
	void send ( char c );
	char get ( void );
	void print ( char ch );
	void print ( int num );
	void print ( char const* str );
	void crlf ( void );
	void init ( int channel, int baud );
	/**/	
	int recvsize;	
	char buf[8];
	int channel;
	USART_TypeDef* Reg;
};
}
