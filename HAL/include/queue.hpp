#pragma once

#include <stdint.h>

#define QUEUE_ELEMENTS 100
#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)

int
QueuePut (char toadd);
int
QueueGet (char *old);
void
QueueInit (void);

