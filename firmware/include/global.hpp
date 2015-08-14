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
#include "gps.hpp"
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

#define OFF false
#define ON true

#define LED C,13,SPEED_50MHz
#define SWEEP A,6,SPEED_50MHz
#define BLINK (PIN_TOGGLE ( LED ))

#define RX1 A,10,SPEED_50MHz
#define TX1 A,9,SPEED_50MHz
#define RX2 A,3,SPEED_50MHz
#define TX2 A,2,SPEED_50MHz
#define RX3 B,11,SPEED_50MHz
#define TX3 B,10,SPEED_50MHz

void assert ( int value );
void freopen ( uart::Uart what, uart::Uart where );

extern uart::Stack usart2data;

extern uint32_t tickcounter;
extern uint32_t uarttimeout;

