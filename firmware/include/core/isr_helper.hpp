#pragma once

/* entering critical section, save priority mask, disable interrupts  */
#define ENTER_CRITICAL \
		word mask=0;\
		__ASM volatile ("MRS %0, primask" : "=r" (mask) );\
		__disable_irq();

/* restore priority mask, force IRQ query. No need for "cpsie i" here */
#define EXIT_CRITICAL \
		__ASM volatile ("MSR primask, %0" : : "r" (mask) );\
		__ISB();


extern "C"
{
 void USART1_IRQHandler (void);
 void USART2_IRQHandler (void);
 void USART3_IRQHandler (void);

 void SPI1_IRQHandler (void);
 void SPI2_IRQHandler (void);
}

inline bool is_in_interrupt();
void remap_vector_table (void);
void hardware_manager_init(void);

enum
{
	USART1_HANDLER, USART2_HANDLER, USART3_HANDLER, USART4_HANDLER
};
