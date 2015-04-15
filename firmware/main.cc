#include <global.hpp>

int main ( void )
{
	uart::Uart uart1 ( 1,1 );

__ASM volatile ( "cpsie i" : : : "memory" );
	//uprint_str ( "\r\nMCU started\r\n" );
	PROGRAM_END;
}
