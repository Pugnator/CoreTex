#pragma once
#include <drivers/console.hpp>
#include <config.hpp>
#include <SEGGER_RTT.h>
#include <SEGGER_SYSVIEW.h>

#define LOGGING_CHANNEL 0


void swo_print(const char *msg);

#define RTTPRINT(string) SEGGER_RTT_WriteString(LOGGING_CHANNEL, string)

