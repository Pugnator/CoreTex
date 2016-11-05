#pragma once
#include <drivers/console.hpp>
#include <config.hpp>
#include <SEGGER_RTT.h>

#ifdef __USE_CONSOLE
extern class Console *__dbg_out;
#endif

void swo_print(const char *msg);

#if __DEBUG
#define DBGPRINT(format, ...) do{__dbg_out->xprintf(format, ##__VA_ARGS__);}while(0)
#else
#define DBGPRINT(format, ...)
#endif

#define LOGPRINT(format, ...) do{__dbg_out->xprintf(format, ##__VA_ARGS__);}while(0)


#define RTTPRINT(string) SEGGER_RTT_WriteString(0, string)

#define RTTPRINTF(format, ...) SEGGER_RTT_printf(0, ##__VA_ARGS__)

