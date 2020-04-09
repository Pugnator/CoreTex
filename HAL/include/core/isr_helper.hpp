#pragma once

extern volatile uint32_t __attribute__((section(".vectorsSection"))) HARDWARE_TABLE[76];

/* entering critical section, save priority mask, disable interrupts  */
#define ENTER_CRITICAL \
		word mask=0;\
		__ASM volatile ("MRS %0, primask" : "=r" (mask) );\
		__disable_irq();

/* restore priority mask, force IRQ query. No need for "cpsie i" here */
#define EXIT_CRITICAL \
		__ASM volatile ("MSR primask, %0" : : "r" (mask) );\
		__ISB();


/* Interrupt vector table enumerators */
typedef enum ISR_VECTOR
{
  RESET_EX = 0,
  NMI_EX,
  HARDFAULT_EX,
  MMU_EX,
  BUS_EX,
  USAGE_EX,
  SVCALL_EX = 11,
  PENDSV_EX = 14,
  SYSTICK_EX,
  IRQ0_EX,
  IRQ1_EX,
  IRQ2_EX,
  IRQ3_EX,
  IRQ4_EX,
  IRQ5_EX,
  IRQ6_EX,
  IRQ7_EX,
  IRQ8_EX,
  IRQ9_EX,
  IRQ10_EX,
  IRQ11_EX,
  IRQ12_EX,
  IRQ13_EX,
  IRQ14_EX,
  IRQ15_EX,
  IRQ16_EX,
  IRQ17_EX,
  IRQ18_EX,
  IRQ19_EX,
  IRQ20_EX,
  IRQ21_EX,
  IRQ22_EX,
  IRQ23_EX,
  IRQ24_EX,
  IRQ25_EX,
  IRQ26_EX,
  IRQ27_EX,
  IRQ28_EX,
  IRQ29_EX,
  IRQ30_EX,
  IRQ31_EX,
  IRQ32_EX,
  IRQ33_EX,
  IRQ34_EX,
  IRQ35_EX,
  IRQ36_EX,
  IRQ37_EX,
  IRQ38_EX,
  IRQ39_EX,
  IRQ40_EX,
  IRQ41_EX,
  IRQ42_EX,
  IRQ43_EX,
  IRQ44_EX,
  IRQ45_EX,
  IRQ46_EX,
  IRQ47_EX,
  IRQ48_EX,
  IRQ49_EX,
  IRQ50_EX,
  IRQ51_EX,
  IRQ52_EX,
  IRQ53_EX,
  IRQ54_EX,
  IRQ55_EX,
  IRQ56_EX,
  IRQ57_EX,
  IRQ58_EX,
  IRQ59_EX,
  IRQ60_EX,
  IRQ61_EX,
  IRQ62_EX,
  IRQ63_EX,
  IRQ64_EX,
  IRQ65_EX,
  IRQ66_EX,
  IRQ67_EX
} ISR_VECTOR;


