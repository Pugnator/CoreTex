#pragma once
#include <global.hpp>
#include <core/usart.hpp>
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
 word deg;
 word min;
 double sec;
 char dir;
 bool valid;
} coord;

typedef struct
{
 int16_t deg;
 word fract;
} UTM;

//TODO: describe the fields
typedef struct nmeactx
{
 word utc;
 coord lat;
 coord lon;
 word msl;
 char fstr[16];
 word knots;
 word kmh;
 float course;
 int alt;
 bool isvalid;
 word nmeaerr;
 word sect; 
 char* fp;
 bool sumdone;
 bool nmeaok;
 uint8_t checksum;
} nmeactx;

class Gps: public USART
{
public:
 Gps(short ch, word bd) :
  USART::USART(ch, bd, this)
 {
  gsv = 0;
  reset();
 }
 void rttprint();
 NMEAERR prepare(void);
 virtual void isr(word address);
 void reset(void);
 char nmeastr[NMEA_MAX_LEN + 1];
 volatile uint8_t nmeastr_len;
 coord getlat();
 coord getlon();
 word get_utc();
 int get_alt();
 word get_speed();
 double get_dec_lat();
 double get_dec_lon();
 UTM coord2utm (coord coord);
 bool ok();
 bool correct_rtc();
 word speed;
 word gsv;

private:
 bool correct;
 volatile bool ready;

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
 void fillGSActx(int sect, const char* field);

 nmeactx nmea;
 NMEATYPE type;
};

