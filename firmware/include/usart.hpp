#pragma once

namespace uart
{
class Uart
{
public:
	Uart ( int ch, int bd, bool doinit );
	~Uart ( void );
	void ( Uart:: *init ) ();
	void ( Uart:: *sendchr ) ( char );
	char ( Uart:: *getchr ) ();
	void print ( char ch );
	void print ( int num );
	void print ( char const* str );
private:
	void stm32_init1 ( void );
	void stm32_init2 ( void );
	void stm32_sendchr1 ( char c );
	char stm32_getchr1 ( void );
	void stm32_sendchr2 ( char c );
	char stm32_getchr2 ( void );
	int channel;
	int baud;
	char buf[16];
};
}
