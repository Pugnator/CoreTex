#pragma once
#include <stdint.h>
#include <global.hpp>

void delay_ms (uint32_t ms);
void delay (uint32_t s);
word str16_to_word (const char* str);
word str10_to_word (const char* str);
char *num2str(int i);
char *strclone(const char *msg);
char *ucs2ascii (const char *ucs2);
uint32_t xorshift();