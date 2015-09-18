#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <parser.h>

#pragma GCC diagnostic ignored "-Wswitch"

uint32_t str16_to_uint ( char const* str );
uint32_t str10_to_uint ( char const* str );
void latlon2crd (const char *str, coord *c);
bool ckecknmea ( uint8_t sum, char* string );

#include <gga.h>
#include <vtg.h>
#include <rmc.h>
