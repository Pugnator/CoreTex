#pragma once

#define STACK_DEPTH 128

namespace uart
{

class Uart;

/**
 * @brief [brief description]
 * @details [long description]
 * @return [description]
 */
class Stack
{

public:
	Stack(void)
	{
		reset();
	};
	void operator+ ( char c );
	char get();
	int len(void)
	{
		return stackp;
	};
	void reset(void)
	{
		stackp = -1;
		ready = false;
	};
	void operator>> ( Uart port );
	char *str();
	bool ready;
	char stack[STACK_DEPTH + 1];
private:
	int stackp;
	int push ( char c );
	int pop ( char* c );

};

/**
 * @brief [brief description]
 * @details [long description]
 *
 * @param ch [description]
 * @param bd [description]
 * @param doinit [description]
 * @return [description]
 */
class Uart
{

public:
	Uart ( int ch, int bd = 9600, bool doinit = false );
	void disable ( void );
	template <typename T> Uart& operator<< ( T arg )
	{
		print ( arg );
		return *this;
	};
	template <typename T> Uart& operator< ( T arg )
	{
		print ( arg );
		crlf();
		return *this;
	};
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
