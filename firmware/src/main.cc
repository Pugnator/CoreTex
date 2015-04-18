#include <global.hpp>

using namespace uart;

#define NL(port) (port.print("\r\n"))

RMC rmc;
void assert ( int value )
{
	;
}

void printGPS ( Uart port )
{
	port.print("UTC: ");
	port.print(rmc.utc);
}



int main ( void )
{

__ASM volatile ( "cpsie i" : : : "memory" );
	rmc.utc = 15;
	Uart dbgout ( 1,115200, true );
	Uart gps ( 2,115200, true ); //Channel 2, 115200 baud, init = true	
	dbgout.print ( WELCOME_TEXT );	
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);
	printGPS(dbgout);
	PROGRAM_END;
}
