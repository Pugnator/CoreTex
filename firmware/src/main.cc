#include <global.hpp>

using namespace uart;
using namespace mcu;

#define NL(port) (port.print("\r\n"))

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
	Mcu stm;
	dbgout += WELCOME_TEXT;			
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);
	//printGPS(dbgout);
	PROGRAM_END;
}
