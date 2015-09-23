#include <global.hpp>

/* UART handling class */

using namespace uart;

Uart::Uart ( int ch, int bd, bool doinit )
{

}

void
Uart::disable ( void )
{

}

void
Uart::send ( char ch )
{

}

char
Uart::get ( void )
{

}

void
Uart::init ( int channel, int baud )
{

}

void
Uart::print ( char ch )
{
	send ( ch );
}

void
Uart::print ( char const* str )
{
	char const* p = str;
	while ( *p )
	{
		send ( *p++ );
	}
}

void
Uart::print ( int num )
{
	int temp = num;
	if ( num > INT_MAX || num < 0 )
	{
		print ( "Can't display number" );
		return;
	}
	
	int dcount = -1;
	/* Calculate number of digits */
	while ( 0!= temp )
	{
		temp /= 10;
		++ dcount;
	}
	
	temp = num;
	buf[dcount+1] = '\0';
	for ( int i = dcount; i>=0 ; i-- )
	{
		buf[i] = ( temp % 10 ) + '0';
		temp /= 10;
	}
	
	print ( buf );
}

void
Uart::crlf ( void )
{
	print ( "\r\n" );
}

/* Stack */

int Stack::push ( char c )
{
	if ( 0 == c )
		return 1;
		
	if ( stackp < STACK_DEPTH - 1 )
	{
		stack[++stackp] = c;
		return 0;
	}
	ready = true;
	return 1;
}

int Stack::pop ( char* c )
{
	if ( stackp >= 0 )
	{
		*c = stack[stackp--];
		return 0;
	}
	return 1;
}

void Stack::operator>> ( Uart port )
{
	for ( int i=1; i < stackp; i++ )
	{
		port < stack[i];
	}
	
}

char Stack::get ( void )
{
	char res;
	pop ( &res );
	return res;
}

void Stack::operator+ ( char c )
{
	push ( c );
}


char* Stack::str ( void )
{
	return stack ? stack : ( char* ) "";
}

