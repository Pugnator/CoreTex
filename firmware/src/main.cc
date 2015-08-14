#include <global.hpp>

using namespace uart;
Stack usart2data;

#define SCK A,5,SPEED_10MHz
#define MOSI A,7,SPEED_10MHz
#define MISO A,6,SPEED_10MHz
#define CS A,4,SPEED_10MHz

void assert ( int value )
{
	if(!value)
		NVIC_SystemReset();
}

void check_last_reset_state (void)
{	
	if (RCC->CSR & RCC_CSR_SFTRSTF)
	{

	}
	else if (RCC->CSR & RCC_CSR_PINRSTF)
	{
		NVIC_SystemReset();
	}	
	else if (RCC->CSR & RCC_CSR_LPWRRSTF)
	{
		NVIC_SystemReset();
	}
	else if (RCC->CSR & RCC_CSR_PORRSTF)
	{
		NVIC_SystemReset();
	}	
}

int main ( void )
{
	__ASM volatile ( "cpsie i" : : : "memory" );	
	PIN_LOW ( LED );
	PIN_LOW ( SWEEP );
	Uart dbgout ( 1, 115200, true );
	Uart gsm ( 2, 19200, true );
	Uart gps ( 3, 115200, true );
	dbgout.cls();
	dbgout.cursor ( OFF );
	usart2data.reset();
	dbgout < WELCOME_TEXT;	
	dbgout < "===========";	
	
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);

	//RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	
	//SCK and MOSI with default alternate function (not re-mapped) push-pull
	/*PIN_OUT_ALT_PP ( SCK );
	PIN_OUT_ALT_PP ( MOSI );
	// Configure MISO as Input with internal pull-up 
	PIN_INPUT_PU ( MISO );
	PIN_OUT_PP ( CS );
	SPI1 -> CR1 = 0;
	SPI1->CR1 |= SPI_CR1_BR
			  | SPI_CR1_SSM
			  | SPI_CR1_SSI
			  | SPI_CR1_MSTR
			  | SPI_CR1_SPE;
	
	SPI1->CR2 |= SPI_CR2_RXNEIE;
	//NVIC_EnableIRQ ( ( IRQn_Type ) SPI1_IRQn );
	//NVIC_SetPriority ( ( IRQn_Type ) SPI1_IRQn, 2 );
	
	dbgout < "SPI inited";	*/							
	//sendSMS (gps, "+79670769685", "This is the text message");
	for ( ;; )
	{
		BLINK;
		delay_ms(1500);
		
		//morse_print("StratoProbe-1, Alt 32599, Spd 12, VSI -2");
	}
}
