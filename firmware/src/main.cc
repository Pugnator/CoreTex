#include <global.hpp>

using namespace uart;
Stack usart2data;

void assert ( int value )
{
	NVIC_SystemReset();
}

int main ( void )
{
__ASM volatile ( "cpsie i" : : : "memory" );
	
	Uart dbgout ( 1, 115200, true );
	Uart gps ( 2, 115200, true );
	dbgout.cls();
	dbgout > WELCOME_TEXT;
	while(false == usart2data.full);
	usart2data >> dbgout;
	//char res = 0;
	//gps.recv(&res, 0);
	//delay(2);
	//dbgout = gps.data.get();
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);
	PROGRAM_END;
}

