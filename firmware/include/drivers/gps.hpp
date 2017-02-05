#pragma once
#include <global.hpp>
#include <core/usart.hpp>

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

typedef enum NMEAERR
{
 NMEA_ERROR_OK = 0, NMEA_GENERIC_ERROR, NMEA_NOT_READY, NMEA_UNKNOWN_TALKER, NMEA_NOT_BEGINNING,
 NMEA_DIAGNOSTIC_MSG
} NMEAERR;

typedef enum NMEATALKER
{
 GP = 1, GL, GN, PMTK
} NMEATALKER;

typedef enum NMEATYPE
{
 GGA = 1, GSV, VTG, RMC, GSA, WRONG
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
 word sec;
 char dir;
} coord;

//TODO: describe the fields
typedef struct nmeactx
{
 word utc;
 coord lat;
 coord lon;
 word msl;
 double knots;
 double kmh;
 float course;
 bool isvalid;
 word nmeaerr;
 word sect;
 char fstr[16];
 char* fp;
 uint8_t checksum;
 bool sumdone;
 bool nmeaok;
} nmeactx;

class Gps: public Uart
{
public:
 Gps(short ch, word bd) :
  Uart::Uart(ch, bd, &gpsisr)
 {
  Gps::self = this;
  reset();
 }
 void rttprint();
 NMEAERR prepare(void);
 static void gpsisr(void);
 void reset(void);
 static class Gps *self;
 bool ready;
 char nmeastr[NMEA_MAX_LEN + 1];
 volatile uint8_t nmeastr_len;
 coord getlat();
 coord getlon();
 double get_dec_lat();
 double get_dec_lon();

private:
 bool correct;
 NMEATYPE get_nmea_sent_type(const char* field);
 NMEATALKER get_nmea_talker(const char* field);
 void latlon2crd(const char* str, coord* c);
 bool ckecknmea(uint8_t sum, char* string);
 NMEAERR parseNMEA(char c);
 void fillGGActx(int sect, const char* field);
 void fillVTGctx(int sect, const char* field);
 void fillRMCctx(int sect, const char* field);
 nmeactx nmea;
 NMEATYPE type;
};
