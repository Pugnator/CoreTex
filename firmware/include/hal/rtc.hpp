#pragma once
#include <drivers/console.hpp>

namespace PIT
{

typedef enum RTC_UTC_OFFSET
{
	ZONE_YANKEE = -12,
	ZONE_XRAY,
	ZONE_WHISKEY,
	ZONE_VICTOR,
	ZONE_UNIFORM,
	ZONE_TANGO,
	ZONE_SIERRA,
	ZONE_ROMEO,
	ZONE_QUEBEC,
	ZONE_PAPA,
	ZONE_OSCAR,
	ZONE_NOVEMBER,
	ZONE_ZULU, //+-0
	ZONE_ALPHA,
	ZONE_BRAVO,
	ZONE_CHARLIE,
	ZONE_DELTA,
	ZONE_ECHO,
	ZONE_FOXTROT,
	ZONE_GOLF,
	ZONE_HOTEL,
	ZONE_INDIA,
	ZONE_KILO,
	ZONE_LIMA,
	ZONE_MIKE //+12
}RTC_UTC_OFFSET;

typedef enum RTCERROR
{
	RTC_OK, RTC_IN_THE_PAST
}RTCERROR;

static const uint16_t days[4][12] =
{
{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 },
{ 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700 },
{ 731, 762, 790, 821, 851, 882, 912, 943, 974, 1004, 1035, 1065 },
{ 1096, 1127, 1155, 1186, 1216, 1247, 1277, 1308, 1339, 1369, 1400, 1430 }, };

typedef struct datetime_t
{
	char year;
	char month;
	char day;
	char hour;
	char minute;
	char second;
} datetime_t;

class Rtc : public Driver
{
public:
	Rtc(word epoch = 0);
	word get();
	short gety();
	char getd();
	char getmn();
	char geth();
	char getm();
	char gets();
	void print(CONSOLE::Console& output);
	void set(word epoch);
	void clear(void);
	word state(void);
private:
	word State;
	datetime_t curdate;
	void
	init(word epoch = 0);
	void
	epoch_to_date(datetime_t* date_time, word epoch = 0);
};
}
