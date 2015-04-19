#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include "io_macro.hpp"

#include "usart.hpp"
#include "log.hpp"
#include "spi.hpp"
#include "gps.hpp"
#include "gsm.hpp"
#include "i2c.hpp"
#include "rtc.hpp"
#include <common.hpp>
#include "timers.hpp"
#include "sdcard.hpp"
#include "config.hpp"
#include "text.hpp"


/* FAT32 */
#include "ff.h"
#include "diskio.h"

#define LED C,13,SPEED_50MHz

#define RX1 A,10,SPEED_50MHz
#define TX1 A,9,SPEED_50MHz
#define RX2 A,3,SPEED_50MHz
#define TX2 A,2,SPEED_50MHz

void assert ( int value );

typedef struct RMC
{
	float lat;
	float lon;
	int utc;
} RMC;

extern RMC rmc;

typedef struct __attribute__((packed))
{
	volatile int __estack;
	volatile int _Reset_Handler;
	volatile int _NMI_Handler;
	volatile int _HardFault_Handler;
	volatile int _MemManage_Handler;
	volatile int _BusFault_Handler;
	volatile int _UsageFault_Handler;
	volatile int _dummy[4] __attribute__((packed));	
	volatile int _SVC_Handler;
	volatile int _DebugMon_Handler;
	volatile int _dummy2;
	volatile int _PendSV_Handler;
	volatile int _SysTick_Handler;
	volatile int _WWDG_IRQHandler;
	volatile int _PVD_IRQHandler;
	volatile int _TAMPER_IRQHandler;
	volatile int _RTC_IRQHandler;
	volatile int _FLASH_IRQHandler;
	volatile int _RCC_IRQHandler;
	volatile int _EXTI0_IRQHandler;
	volatile int _EXTI1_IRQHandler;
	volatile int _EXTI2_IRQHandler;
	volatile int _EXTI3_IRQHandler;
	volatile int _EXTI4_IRQHandler;
	volatile int _DMA1_Channel1_IRQHandler;
	volatile int _DMA1_Channel2_IRQHandler;
	volatile int _DMA1_Channel3_IRQHandler;
	volatile int _DMA1_Channel4_IRQHandler;
	volatile int _DMA1_Channel5_IRQHandler;
	volatile int _DMA1_Channel6_IRQHandler;
	volatile int _DMA1_Channel7_IRQHandler;
	volatile int _ADC1_2_IRQHandler;
	volatile int _USB_HP_CAN1_TX_IRQHandler;
	volatile int _USB_LP_CAN1_RX0_IRQHandler;
	volatile int _CAN1_RX1_IRQHandler;
	volatile int _CAN1_SCE_IRQHandler;
	volatile int _EXTI9_5_IRQHandler;
	volatile int _TIM1_BRK_IRQHandler;
	volatile int _TIM1_UP_IRQHandler;
	volatile int _TIM1_TRG_COM_IRQHandler;
	volatile int _TIM1_CC_IRQHandler;
	volatile int _TIM2_IRQHandler;
	volatile int _TIM3_IRQHandler;
	volatile int _TIM4_IRQHandler;
	volatile int _I2C1_EV_IRQHandler;
	volatile int _I2C1_ER_IRQHandler;
	volatile int _I2C2_EV_IRQHandler;
	volatile int _I2C2_ER_IRQHandler;
	volatile int _SPI1_IRQHandler;
	volatile int _SPI2_IRQHandler;
	volatile int _USART1_IRQHandler;
	volatile int _USART2_IRQHandler;
	volatile int _USART3_IRQHandler;
	volatile int _EXTI15_10_IRQHandler;
	volatile int _RTCAlarm_IRQHandler;
	volatile int _USBWakeUp_IRQHandler;
  	volatile int _dummy3[7] __attribute__((packed));
	volatile int _BootRAM;         
}VECTORS;

extern VECTORS vectortbl;
