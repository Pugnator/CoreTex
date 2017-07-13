#pragma once

#define CRYSTAL_CLOCK 8000000ul
#define TIMERTICK (CRYSTAL_CLOCK/1000-1) // This will need tweaking or calculating

extern "C" void
SystemInit (void);
