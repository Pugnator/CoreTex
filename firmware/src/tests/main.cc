#include <global.hpp>
#include <common.hpp>
#include <stdlib.h>
#include <hal/vmmu.hpp>
#include <drivers/console.hpp>

bool memtest(word times);

void inline infinite_loop(void)
  {
    for (;;);
  }

int main(void)
  {
    srand(0);
    memtest(1000);
    infinite_loop();
  }
