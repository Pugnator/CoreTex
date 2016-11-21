#pragma once
#include <drivers/console.hpp>
#include <config.hpp>
#include <SEGGER_RTT.h>
#include <SEGGER_SYSVIEW.h>

void swo_print(const char *msg);

#define RTTPRINT(string) SEGGER_RTT_WriteString(0, string)

