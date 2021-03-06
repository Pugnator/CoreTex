#pragma once
#include <global.hpp>
#include <core/uart.hpp>
#include <stdlib.h>

/*
 1 - +3.3
 2 - VBAT
 3 - GND
 4 - TX (out)
 5 - RX (in)
 6 - GND
 */

#pragma GCC diagnostic ignored "-Wswitch"

#define NMEA_MAX_LEN 82

#define TIME_OFFSET 3

typedef enum NMEAERR
{
 NMEA_ERROR_OK = 0, NMEA_GENERIC_ERROR, NMEA_NOT_READY, NMEA_UNKNOWN_TALKER, NMEA_NOT_BEGINNING, NMEA_EMPTY_FIELD,
 NMEA_DIAGNOSTIC_MSG
} NMEAERR;

typedef enum NMEATALKER
{
 GP = 1, GL, GN, PMTK
} NMEATALKER;

typedef enum NMEATYPE
{
 GGA = 1, GSV, VTG, RMC, GSA, GLL, WRONG
} NMEATYPE;

typedef struct NMEATYPESTRUCT
{
 NMEATYPE type;
 const char* str;
} NMEATYPESTRUCT;

typedef struct NMEATALKERSTRUCT
{
 NMEATALKER type;
 const char* str;
} NMEATALKERSTRUCT;

typedef struct coord
{
 uint32_t deg;
 uint32_t min;
 double sec;
 char dir;
 bool valid;
} coord;

typedef struct
{
 uint32_t deg;
 uint32_t fract;
} UTM;

//TODO: describe the fields
typedef struct nmeactx
{
 uint32_t utc;
 coord lat;
 coord lon;
 uint32_t msl;
 char fstr[16];
 uint32_t kmh;
 float course;
 bool isvalid;
 uint32_t nmeaerr;
 uint32_t sect;
 char* fp;
 uint8_t checksum;
 bool sumdone;
 bool nmeaok;
} nmeactx;

class Gps: public UART
{
public:
 Gps(short ch, uint32_t bd) :
  UART::UART(ch, bd, this)
 {
  reset();
 }
 void rttprint();
 NMEAERR prepare(void);
 virtual void isr(uint32_t address);
 void reset(void);
 char nmeastr[NMEA_MAX_LEN + 1];
 volatile uint8_t nmeastr_len;
 coord getlat();
 coord getlon();
 uint32_t get_utc();
 uint32_t get_speed();
 double get_dec_lat();
 double get_dec_lon();
 UTM coord2utm (coord coord);
 bool ok();
 bool correct_rtc();

 uint32_t gsv;

private:
 bool correct;
 volatile bool ready;
 bool fix;

 void latlon2crd(const char* str, coord* c);

 bool ckecknmea(uint8_t sum, char* string);
 NMEATYPE get_nmea_sent_type(const char* field);
 NMEATALKER get_nmea_talker(const char* field);
 NMEAERR parse(char c);

 void fillGGActx(int sect, const char* field);
 void fillVTGctx(int sect, const char* field);
 void fillRMCctx(int sect, const char* field);
 void fillGLLctx(int sect, const char* field);
 void fillGSVctx(int sect, const char* field);

 nmeactx nmea;
 NMEATYPE type;
};

