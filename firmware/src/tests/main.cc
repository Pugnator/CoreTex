#include <global.hpp>
#include <common.hpp>
#include <core/vmmu.hpp>
#include <stdlib.h>
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
