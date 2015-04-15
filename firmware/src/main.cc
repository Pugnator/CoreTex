#include <global.hpp>

int main ( void )
{
	uart::Uart newu ( 1,9600 );
	
//__ASM volatile ( "cpsie i" : : : "memory" );
	newu.print ( "232323" );
	PROGRAM_END;
}
