#include <global.hpp>

using namespace uart;

RMC rmc;
void assert ( int value )
{
	;
}

void printGPS ( Uart port )
{
	port = "UTC: ";
	port = rmc.utc;
}



int main ( void )
{

__ASM volatile ( "cpsie i" : : : "memory" );
	Uart dbgout ( 1, 115200, true );
	Uart gps ( 2, 115200, true );
	dbgout += WELCOME_TEXT;
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);
	//printGPS(dbgout);
	PROGRAM_END;
}
