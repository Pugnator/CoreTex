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
	char gpsbuf[32]={0};
	int counter = 0;
	char res = 0;
	while(1)
	{			
		res = (gps.*gps.uart::Uart::getchr)();
		gpsbuf[counter++] = res;
		if(res == '\n')
		{
			dbgout.print("======================\r\n");	
			gpsbuf[counter] = '\0';
			//dbgout.print(gpsbuf);							
			counter = 0;							
			dbgout.print("======================\r\n");	
		}
		
	}
	PROGRAM_END;
}
