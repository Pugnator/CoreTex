#include <global.hpp>

#define NVIC_VectTab_FLASH      (0x00000000)
#define AP_START_SECTOR          0x00010000

VECTORS vectortbl;

using namespace uart;

void NVIC_SetVectorTable(unsigned long NVIC_VectTab, unsigned long Offset)
{
	SCB->VTOR = NVIC_VectTab | (Offset & (unsigned int)0x1FFFFF80);
}

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

extern "C" void USART2_IRQHandler ( void );
extern "C" void Reset_Handler ( void );
int main ( void )
{
	void (*main_entry)(void);
	int start = (int)&vectortbl;
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, start);	
    main_entry = (void (*)(void))((int *)&vectortbl+1);      
    vectortbl._USART2_IRQHandler = (int)&USART2_IRQHandler;
    vectortbl._Reset_Handler = (int)&Reset_Handler;
	 main_entry();

	Uart dbgout ( 1, 115200, true );
	Uart gps ( 2, 115200, true );
	dbgout += WELCOME_TEXT;		
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);
	//printGPS(dbgout);	
}
