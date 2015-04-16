#include <global.hpp>

void assert ( int value )
{
	;
}

using namespace uart;

int main ( void )
{
__ASM volatile ( "cpsie i" : : : "memory" );
	Uart dbgout ( 1,9600, true ); //Channel 1, 9600 baud, init = true
	Uart gps ( 1,115200, true ); //Channel 1, 9600 baud, init = true
	//(dbgout.*dbgout.uart::Uart::init)();
	dbgout.print ( WELCOME_TEXT );
	PROGRAM_END;
}
