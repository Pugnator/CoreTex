#pragma once
                
#define CRYSTAL 8000000UL
#define TIMERTICK (CRYSTAL/1000-1) // This will need tweaking or calculating
#define PROGRAM_END for(;;)

extern "C"  void SystemInit ( void );
