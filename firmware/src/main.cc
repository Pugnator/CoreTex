#include <global.hpp>

using namespace uart;
Stack nmea;

void assert ( int value )
{
	NVIC_SystemReset();
}

int main ( void )
{
__ASM volatile ( "cpsie i" : : : "memory" );
	nmea.ready = false;
	Uart dbgout ( 1, 115200, true );
	Uart gps ( 2, 115200, true );
	dbgout.cls();
	dbgout = WELCOME_TEXT;
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);
	PROGRAM_END;
}
