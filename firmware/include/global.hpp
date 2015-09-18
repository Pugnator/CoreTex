#pragma once
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "io_macro.hpp"

#include "version.hpp"
#include "usart.hpp"
#include "log.hpp"
#include "spi.hpp"
#include "gsm.hpp"
#include "i2c.hpp"
#include "rtc.hpp"
#include "common.hpp"
#include "timers.hpp"
#include "sdcard.hpp"
#include "config.hpp"
#include "text.hpp"
#include "morse.hpp"


/* FAT32 */
#include "ff.h"
#include "diskio.h"

/* NMEA */
extern "C"
{
	/*
	1 - +3.3
	2 - VBAT
	3 - GND
	4 - TX (out)
	5 - RX (in)
	6 - GND
	*/
#include "parser.h"
}

#define OFF false
#define ON true

#define LED C,13,SPEED_50MHz
#define SWEEP A,6,SPEED_50MHz
#define BLINK (PIN_TOGGLE ( LED ))

/* GSM */
#define GSMDCD A,11,SPEED_50MHz
#define GSMCTS A,0,SPEED_50MHz
#define GSMDTR A,1,SPEED_50MHz

/* UART */
#define RX1 A,10,SPEED_50MHz
#define TX1 A,9,SPEED_50MHz
#define RX2 A,3,SPEED_50MHz
#define TX2 A,2,SPEED_50MHz
#define RX3 B,11,SPEED_50MHz
#define TX3 B,10,SPEED_50MHz

void assert ( int value );
void freopen ( uart::Uart what, uart::Uart where );

extern uint32_t tickcounter;
extern uint32_t uarttimeout;
extern uint32_t gsmtimeout;
extern uart::Stack gsmStack;
