#pragma once
#include <core/io_macro.hpp>
#include <stdint.h>
#include <config.hpp>
#include <setjmp.h>

#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#pragma GCC diagnostic ignored "-Wunused-variable"

#define OFF false
#define ON true

#define CONSOLE_SPEED 9600
extern jmp_buf ex_buf__;
extern volatile uint32_t tickcounter;
extern volatile uint32_t timerms;
extern volatile uint32_t uscounter;

#define WAIT_FOR(ms) tickcounter = ms
#define STILL_WAIT tickcounter

#define USED __attribute__((used))

#define MAIN_END                  \
  do                              \
  {                               \
    PrintF(0, "Program end\r\n"); \
    for (;;)                      \
      ;                           \
  } while (0)

#define delayus_asm(us)                \
  do                                   \
  {                                    \
    asm volatile("MOV R0,%[loops]\n\t" \
                 "1: \n\t"             \
                 "SUB R0, #1\n\t"      \
                 "CMP R0, #0\n\t"      \
                 "BNE 1b \n\t"         \
                 :                     \
                 : [loops] "r"(us)     \
                 : "memory");          \
  } while (0)

#define delayms_asm(us) \
  do                    \
  {                     \
    delayus_asm(1000);  \
  } while (0)


enum
{
  CHANNEL_1 = 1,
  CHANNEL_2,
  CHANNEL_3,
  CHANNEL_4
};