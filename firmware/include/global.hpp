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

#define STACK_DEPTH 16
#define LED C,13,SPEED_50MHz

#define RX1 A,10,SPEED_50MHz
#define TX1 A,9,SPEED_50MHz
#define RX2 A,3,SPEED_50MHz
#define TX2 A,2,SPEED_50MHz

void assert ( int value );

extern char stack[STACK_DEPTH];
extern int stackp;
extern bool nmeaready;

extern uart::Stack nmea;
