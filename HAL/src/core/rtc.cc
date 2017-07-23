/*******************************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 2015, 2016
 *******************************************************************************/

#include <core/rtc.hpp>
#include <core/stm32f10x.hpp>
#include <xprintf.h>
#include <core/vmmu.hpp>
#include <log.hpp>
#include <global.hpp>
#include <stdlib.h>

void
Rtc::init (word epoch)
{
 RCC->APB1ENR |= RCC_APB1ENR_PWREN | RCC_APB1ENR_BKPEN;
 PWR->CR |= PWR_CR_DBP;

 RCC->BDCR |= RCC_BDCR_BDRST;
 RCC->BDCR &= ~RCC_BDCR_BDRST;

 RCC->BDCR &= ~(RCC_BDCR_LSEON | RCC_BDCR_LSEBYP);
 RCC->BDCR |= RCC_BDCR_LSEON;
 while (!(RCC->BDCR & RCC_BDCR_LSERDY))
  ;

 RCC->BDCR |= RCC_BDCR_RTCEN | RCC_BDCR_RTCSEL_0;
 while (!(RTC->CRL & RTC_CRL_RTOFF))
  ;
 RTC->CRL |= RTC_CRL_CNF;
 RTC->PRLH = 0;
 RTC->PRLL = 32767; //32768 - 1, working from external 32768 quarz
 if ( epoch )
 {
  RTC->CNTH = (epoch >> 16) & 0xFFFF;
  RTC->CNTL = epoch & 0xFFFF;
 }

 //RTC->CRH = RTC_CRH_SECIE;
 RTC->CRL &= ~RTC_CRL_CNF;
 while (!(RTC->CRL & RTC_CRL_RTOFF))
  ;
 PWR->CR &= ~PWR_CR_DBP;
 while (!(RTC->CRL & RTC_CRL_RTOFF))
  ;
}

void
Rtc::set (word epoch)
{
 RTC->CNTH = (epoch >> 16) & 0xFFFF;
 RTC->CNTL = epoch & 0xFFFF;
}

word
Rtc::get (void)
{
 return (RTC->CNTH << 16) | (RTC->CNTL & 0xFFFF);
}

word Rtc::date_to_epoch(datetime_t* date_time)
{
    word second = date_time->second;  // 0-59
    word minute = date_time->minute;  // 0-59
    word hour   = date_time->hour;    // 0-23
    word day    = date_time->day-1;   // 0-30
    word month  = date_time->month-1; // 0-11
    word year   = date_time->year;    // 0-99
    return (((year/4*(365*4+1)+days[year%4][month]+day)*24+hour)*60+minute)*60+second;
}


void
Rtc::epoch_to_date (datetime_t* date_time, word epoch)
{
 date_time->second = epoch % 60;
 epoch /= 60;
 date_time->minute = epoch % 60;
 epoch /= 60;
 date_time->hour = epoch % 24;
 epoch /= 24;

 word years = epoch / (365 * 4 + 1) * 4;
 epoch %= 365 * 4 + 1;

 word year;
 for (year = 3; year > 0; year--)
 {
  if ( epoch >= days[year][0] )
   break;
 }

 word month;
 for (month = 11; month > 0; month--)
 {
  if ( epoch >= days[year][month] )
   break;
 }

 date_time->year = years + year;
 date_time->month = month + 1;
 date_time->day = epoch - days[year][month] + 1;
}

/**
 *	@brief: get current year
 */
short
Rtc::gety ()
{
 epoch_to_date (&curdate, get ());
 return 1970 + curdate.year;
}
char
Rtc::getd ()
{
 epoch_to_date (&curdate, get ());
 return curdate.day;
}
char
Rtc::getmn ()
{
 epoch_to_date (&curdate, get ());
 return curdate.month;
}
char
Rtc::geth ()
{
 epoch_to_date (&curdate, get ());
 return curdate.hour + ZONE_CHARLIE;
}
char
Rtc::getm ()
{
 epoch_to_date (&curdate, get ());
 return curdate.minute;
}
char
Rtc::gets ()
{
 epoch_to_date (&curdate, get ());
 return curdate.second;
}

void
Rtc::print ()
{
 SEGGER_RTT_printf (0, "%04u/%02u/%02u %02u:%02u:%02u\n", gety (), getmn (),
                    getd (), geth (), getm (), gets ());
}

char*
Rtc::get_string()
{
 char *str = (char*)ALLOC(20);
 if(!str)
 {
   return nullptr;
 }
 xsprintf (str, "%04u/%02u/%02u %02u:%02u:%02u", gety (), getmn (),
                    getd (), geth (), getm (), gets ());
 return str;
}

char*
Rtc::get_date_string()
{
 char *str = (char*)ALLOC(20);
 if(!str)
 {
   return nullptr;
 }
 xsprintf (str, "%04u-%02u-%02u", gety (), getmn (),
                    getd (), geth (), getm (), gets ());
 return str;
}
