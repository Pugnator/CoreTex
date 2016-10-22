#pragma once
#include <drivers/console.hpp>
#include <config.hpp>

extern Console *__dbg_out;

void dbg_print(const char *msg);

#if __DEBUG
#define DBGPRINT(format, ...) do{__dbg_out->xprintf(format, ##__VA_ARGS__);}while(0)
#else
#define DBGPRINT(format, ...)
#endif

#define LOGPRINT(format, ...) do{__dbg_out->xprintf(format, ##__VA_ARGS__);}while(0)