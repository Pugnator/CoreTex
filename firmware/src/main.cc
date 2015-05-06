#include <global.hpp>

using namespace uart;
Stack usart2data;

#define SCK A,5,SPEED_10MHz
#define MOSI A,7,SPEED_10MHz
#define MISO A,6,SPEED_10MHz
#define CS A,4,SPEED_10MHz
#define NRFCS A,4,SPEED_10MHz

#define MMC_GO_IDLE_STATE   0
#define MMC_SEND_OP_COND    1
#define MMC_READ_OCR        58

void assert ( int value )
{
	NVIC_SystemReset();
}

uint16_t spi1_trx ( uint16_t data )
{
	SPI1->DR = data;
	while ( ( SPI1->SR & SPI_SR_RXNE ) == RESET );
	return SPI1->DR;
}


int main ( void )
{

__ASM volatile ( "cpsie i" : : : "memory" );
	PIN_LOW ( LED );
	Uart dbgout ( 1, 115200, true );
	Uart gps ( 2, 115200, true );
	dbgout.cls();
	dbgout.cursor ( OFF );
	usart2data.reset();
	dbgout < WELCOME_TEXT;
	//FATFS FatFs;
	//f_mount(&FatFs, "", 0);

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	//SCK and MOSI with default alternate function (not re-mapped) push-pull
	PIN_OUT_ALT_PP ( SCK );
	PIN_OUT_ALT_PP ( MOSI );
	/* Configure MISO as Input with internal pull-up */
	PIN_INPUT_PU ( MISO );
	PIN_OUT_PP ( CS );
	SPI1 -> CR1 = 0;
	SPI1->CR1 |= SPI_CR1_BR
			  | SPI_CR1_SSM
			  | SPI_CR1_SSI
			  | SPI_CR1_MSTR
			  | SPI_CR1_SPE;
	
	SPI1->CR2 |= SPI_CR2_RXNEIE;
	NVIC_EnableIRQ ( ( IRQn_Type ) SPI1_IRQn );
	NVIC_SetPriority ( ( IRQn_Type ) SPI1_IRQn, 2 );
	PIN_HI ( CS );
	PIN_HI ( NRFCS );
	
	dbgout < "SPI inited";
	spresponse = 0;
	PIN_LOW ( NRFCS );	
	spresponse = spi1_trx(131);
	PIN_HI ( NRFCS );
	dbgout < (int)spresponse;

	dbgout < "SPI finished";
	for ( ;; )
	{
		/*if ( usart2data.ready )
		{
		    usart2data.reset();
		}*/
	}
}
