#pragma once
#include <stdint.h>
#include <global.hpp>

void delay_ms (uint32_t ms);
void delay (uint32_t s);
void tsprintf (			/* Put a formatted string to the memory */
	char* buff,			/* Pointer to the output buffer */
	const char*	fmt,	/* Pointer to the format string */
	...					/* Optional arguments */
);

bool isNthBitSet(uint8_t c, uint8_t n);
int ascii2ucs2(const char* sms_text, uint8_t* output_buffer, int buffer_size);
uint32_t str16_to_word (const char* str);
uint32_t str10_to_word (const char* str);
char *num2str(int i);
char *strclone(const char *msg);
char *ucs2ascii (const char *ucs2);
uint32_t xorshift();
void asser_param(int condition);
void __assert(int condition, const char *file, int line);
#define ASSERT(X) do{__assert(X, __FILE__, __LINE__);}while(0)
int isprint(char c);
