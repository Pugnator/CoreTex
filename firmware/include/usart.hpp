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
	char len(void){return stackp;};	
	void reset(void){stackp = -1;};	
	void operator>> ( Uart port );	
	char *str();	
	bool full;
private:	
	char stack[STACK_DEPTH + 1];	
	int stackp;
	int push ( char c );
	int pop ( char* c );	
	
};

class Uart
{

public:
	Uart ( int ch, int bd = 9600, bool doinit = false );
	void disable ( void );
	template<typename T> Uart& operator<< ( T x ){ print ( x ); return *this;};
	template<typename T> Uart& operator< ( T x ){ print ( x ); crlf();return *this;};	
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
