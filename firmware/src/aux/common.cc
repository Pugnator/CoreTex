#include <global.hpp>
#include <stdint.h>

uint32_t str16_to_uint ( char const* str )
{
	uint32_t res = 0;
	char c = 0;
	while ( *str )
	{
		c = *str;
		res <<= 4;
		res += ( c > '9' ) ? ( c & 0xDFu ) - 0x37u: ( c - '0' );
		++str;
	}
	return res;
}

uint32_t str10_to_uint ( char const* str )
{
	uint32_t res = 0;
	while ( *str )
	{
		if ( ( *str >= '0' ) && ( *str <= '9' ) )
		{
			res = ( res * 10 ) + ( ( *str ) - '0' );
		}
		else if ( ',' == *str || '.' == *str )
		{
			break;
		}
		str++;
	}
	return res;
}

void delay_ms ( uint32_t ms )
{
	tickcounter = ms;
	while ( tickcounter );
}

void delay ( uint32_t s )
{
	delay_ms ( s*1000 );
}


