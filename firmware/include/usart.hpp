#pragma once

#define STACK_DEPTH 128
#define NMEA_SIZE 82

namespace uart
{

class Uart;
class Stack;

/**
 * @brief [brief description]
 * @details [long description]
 * @return [description]
 */
class Stack
{

public:
	Stack ( void )
	{
		reset();
	};
	void operator+ ( char c );
	char get ( void );
	int len ( void )
	{
		return stackp;
	};
	void reset ( void )
	{
		stackp = -1;
		ready = false;
		memset ( stack, 0, sizeof stack );
	};
	void operator>> ( Uart port );
	char* str ( void );
	char top ( void )
	{
		return stack[stackp];
	}
	bool ready;
private:
	char stack[STACK_DEPTH + 1];
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
	template <typename T> Uart& operator<< (  T arg )
	{
		print ( arg );
		return *this;
	};
	template <typename T> Uart& operator< (  T arg )
	{
		print ( arg );
		crlf();
		return *this;
	};
	void recv ( char* c, int timeout = 0 );
	void cls ( void )
	{
		print ( "\x1B[2J\x1B[H" );
	}
	void cursor ( bool state = true )
	{
		if ( false == state )
		{
			print ( "\e[?25l" );
		}
		else
		{
			print ( "\e[?25h" );
		}
	}
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

class NMEA
{
public:
	NMEA ( char* str )
	{
		strcpy ( nmea, str );
	}
	bool valid ( void )
	{
		return isGPGGA();
	}
	void parse ( void );
	bool state ( void );
	int satnum ( void );
	double lon ( void );
	double lat ( void );
	int utc ( void );
	int msl ( void );
	int alt ( void );
	int chksum ( void );
	void operator= ( char* str );
	char str ( void );
protected:
	char nmea[NMEA_SIZE];
private:
	bool isGPGGA ( void )
	{
		return strncmp ( nmea, "$GPGGA", 6 ) ? false : true;
		//TODO: checksum
	}
};
}
