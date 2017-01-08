#pragma once
#include <stdint.h>
#include <global.hpp>

void delay_ms (uint32_t ms);
void delay (uint32_t s);
int ascii2ucs2(const char* sms_text, uint8_t* output_buffer, int buffer_size);
word str16_to_word (const char* str);
word str10_to_word (const char* str);
char *num2str(int i);
char *strclone(const char *msg);
char *ucs2ascii (const char *ucs2);
uint32_t xorshift();
void __assert(int condition, const char *file, int line);
#define ASSERT(X) do{__assert(X, __FILE__, __LINE__);}while(0)
int isprint(char c);
