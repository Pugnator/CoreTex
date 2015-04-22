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
	PIN_LOW(LED);
	Uart dbgout ( 1, 115200, true );
	Uart gps ( 2, 115200, true );
	dbgout.cls();	
	dbgout < WELCOME_TEXT;
	while(false == usart2data.full);
	dbgout << "Stack size is " << (int)usart2data.len() < " bytes";
	//char res = 0;
	//gps.recv(&res, 0);
	//delay(2);
	//dbgout = gps.data.get();
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);
	PROGRAM_END;
}

