#include <global.hpp>

using namespace uart;
Stack usart2data;

void assert ( int value )
{
	NVIC_SystemReset();
}

bool isGPGGA (char *nmea)
{
	return strncmp(nmea, "$GPGGA", 6) ? false : true;
}

int main ( void )
{
	
	__ASM volatile ( "cpsie i" : : : "memory" );
	PIN_LOW(LED);
	Uart dbgout ( 1, 115200, true );
	Uart gps ( 2, 115200, true );
	dbgout.cls();	
	usart2data.reset();
	dbgout < WELCOME_TEXT;	
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);
	for(;;)
	{
		if(usart2data.ready)
		{			
			/* Check if string inside stack is GPGGA */			
			if(isGPGGA(usart2data.stack))
			{
				dbgout < "NMEA: $GPGGA";
			}
			else
			{
				dbgout << usart2data.stack;
			}
			usart2data.reset();									
		}
	}
}

