#include <global.hpp>

void delay_ms ( int ms )
{
	volatile uint64_t nCount= ( CRYSTAL/10000/2 ) *ms;
	for ( ; nCount!=0; nCount-- );
}

void delay ( int s )
{
	delay_ms ( s*1000 );
}

using namespace Common;


Error::Error(void)
{
	status = false;
}


