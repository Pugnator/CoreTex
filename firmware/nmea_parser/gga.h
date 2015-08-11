#pragma once
#include <global.h>

typedef enum NMEAFORMAT
{
    TYPE , UTC, LAT, LATDIR, LONG, LONGDIR, QUAL, SATNUM, HDOP, MSL, ALT, GEOSEP, GEOSEPU, AGE, ID, CS, END
} NMEAFORMAT;

void fillGGActx (int sect, const char *field);