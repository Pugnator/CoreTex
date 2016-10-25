#pragma once
#include <drivers/console.hpp>
#include <config.hpp>

#ifdef __USE_CONSOLE
extern class Console *__dbg_out;
#endif

void dbg_print(const char *msg);

#if __DEBUG
#define DBGPRINT(format, ...) do{__dbg_out->xprintf(format, ##__VA_ARGS__);}while(0)
#else
#define DBGPRINT(format, ...)
#endif

#define LOGPRINT(format, ...) do{__dbg_out->xprintf(format, ##__VA_ARGS__);}while(0)

#define LOGDUMP(buf, size) do{__dbg_out->put_dump(buf, 0, size, 1);}while(0)
