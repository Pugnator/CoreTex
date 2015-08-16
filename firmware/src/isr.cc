#include <global.hpp>
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"

extern "C"
{
	void SysTick_Handler ( void )
	{
		if(tickcounter)
		{
			--tickcounter;	
		}		
	}
	
	void SPI1_IRQHandler ( void )
	{
		if ( SET == ( SPI1->SR & SPI_SR_RXNE ) )
		{
			SPI1->SR&=~  SPI_SR_RXNE;		
			//spresponse = SPI1->DR;
		}		
	}
	
	/* Camera/debug port */
	
	void USART1_IRQHandler ( void )
	{
		if ( USART1->SR & USART_SR_RXNE ) //receive
		{
			char c = USART2->DR;
			USART1->DR = c;
			USART1->SR&= ~USART_SR_RXNE;;
		}
		else if ( USART1->SR & USART_SR_TC ) //transfer
		{
			USART1->SR&= ~USART_SR_TC;
		}		
	}
	
	/* GSM port */
	
	void USART2_IRQHandler ( void )
	{
		if ( USART2->SR & USART_SR_RXNE ) //receive
		{
			char c = USART2->DR;
			#ifdef DEBUG
			//USART1->DR = c;			
			#endif
			if ( '+' == c && !gsmStack.ready)
			{
				gsmStack.reset();
			}
			else if ( '\r' == c && !gsmStack.ready)
			{
				if(strstr(gsmStack.str(), "\r\nOK"))
				{
					gsmStack.ready = true;	
				}
				else
				{
					gsmStack + c;
				}				
			}
			else if ( !gsmStack.ready )
			{				
				gsmStack + c;
			}
			USART2->SR&= ~USART_SR_RXNE;
		}
		else if ( USART2->SR & USART_SR_TC ) //transfer
		{
			USART2->SR&= ~USART_SR_TC;
		}		
	}
	
	/* GPS port */
	void USART3_IRQHandler ( void )
	{
		if ( USART3->SR & USART_SR_RXNE ) //receive
		{
			char c = USART3->DR;
			USART1->DR = c;
			USART3->SR&= ~USART_SR_RXNE;
		}
		else if ( USART3->SR & USART_SR_TC ) //transfer
		{
			USART3->SR&= ~USART_SR_TC;
		}
		
	}
	

	//TO USE: addr2len /path/to/elf 0x800186d [GDB: p/x pc when it hit for(;;)]
	void unwindCPUstack ( uint32_t* stackAddress )
	{
		/*
		These are volatile to try and prevent the compiler/linker optimising them
		away as the variables never actually get used.  If the debugger won't show the
		values of the variables, make them global my moving their declaration outside
		of this function.
		*/
		volatile uint32_t r0;
		volatile uint32_t r1;
		volatile uint32_t r2;
		volatile uint32_t r3;
		volatile uint32_t r12;
		volatile uint32_t lr; /* Link register. */
		volatile uint32_t pc; /* Program counter. */
		volatile uint32_t psr;/* Program status register. */
		
		r0 = stackAddress[0];
		r1 = stackAddress[1];
		r2 = stackAddress[2];
		r3 = stackAddress[3];
		
		r12 = stackAddress[4];
		lr = stackAddress[5];
		pc = stackAddress[6];
		psr = stackAddress[7];
		
		/* When the following line is hit, the variables contain the register values. */
		for ( ;; )
		{
		#ifdef DEBUG
			PIN_HI(LED);		
		#else
			NVIC_SystemReset();
		#endif	
		}
	}
	
	
	
	void HardFault_Handler ( void )
	{
		__asm volatile
		(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, handler_address_const                            \n"
			" bx r2                                                     \n"
			" handler_address_const: .word unwindCPUstack    \n"
		);
	}
	
	void MemManage_Handler ( void )
	{
		__asm volatile
		(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, handler2_address_const                            \n"
			" bx r2                                                     \n"
			" handler2_address_const: .word unwindCPUstack    \n"
		);
	}
	
	void BusFault_Handler ( void )
	{
		__asm volatile
		(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, handler3_address_const                            \n"
			" bx r2                                                     \n"
			" handler3_address_const: .word unwindCPUstack    \n"
		);
	}
	
	void UsageFault_Handler ( void )
	{
		__asm volatile
		(
			" tst lr, #4                                                \n"
			" ite eq                                                    \n"
			" mrseq r0, msp                                             \n"
			" mrsne r0, psp                                             \n"
			" ldr r1, [r0, #24]                                         \n"
			" ldr r2, handler4_address_const                            \n"
			" bx r2                                                     \n"
			" handler4_address_const: .word unwindCPUstack    \n"
		);
	}
}
