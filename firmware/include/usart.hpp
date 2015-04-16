#pragma once

namespace uart
{
class Uart
{
public:
	Uart ( int ch, int bd, bool doinit );
	~Uart ( void );
	void print ( char ch );
	void print ( int num );
	void print ( char const* str );
private:
	void stm32_init ( void );
	void stm32_sendchr ( char c );
	char stm32_getchr ( void );
	int channel;
	int baud;
	char buf[16];
	USART_TypeDef* Reg;
};
}
