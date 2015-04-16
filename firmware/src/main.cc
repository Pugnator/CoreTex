#include <global.hpp>

void assert ( int value )
{
	;
}

using namespace uart;

int main ( void )
{
__ASM volatile ( "cpsie i" : : : "memory" );
	Uart dbgout ( 1,115200, true ); //Channel 1, 115200 baud, init = true
	Uart gps ( 2,115200, true ); //Channel 2, 115200 baud, init = true
	dbgout.print ( WELCOME_TEXT );
	FATFS FatFs;
	f_mount(&FatFs, "", 0);

	PROGRAM_END;
}
