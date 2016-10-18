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
#include <global.hpp>
#include <stdlib.h>

Rtc::Rtc(word epoch)
{
	State = 0;
	if (epoch)
	{
		init(epoch);
	}
	else
	{
		if (1970 == gety())
		{
			State = RTC_IN_THE_PAST;
		}
		return;
	}
}
;

void Rtc::init(word epoch)
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
	RTC->CNTH = (epoch >> 16) & 0xFFFF;
	RTC->CNTL = epoch & 0xFFFF;
	/**/
	//RTC->CRH = RTC_CRH_SECIE;
	RTC->CRL &= ~RTC_CRL_CNF;
	while (!(RTC->CRL & RTC_CRL_RTOFF))
		;
	PWR->CR &= ~PWR_CR_DBP;
	while (!(RTC->CRL & RTC_CRL_RTOFF))
		;
}

//FIXME: hangs here for some reason
void Rtc::set(word epoch)
{
	init(epoch);
}

word Rtc::get(void)
{
	return (RTC->CNTH << 16) | (RTC->CNTL & 0xFFFF);
}

void Rtc::epoch_to_date(datetime_t* date_time, word epoch)
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
		if (epoch >= days[year][0])
			break;
	}

	word month;
	for (month = 11; month > 0; month--)
	{
		if (epoch >= days[year][month])
			break;
	}

	date_time->year = years + year;
	date_time->month = month + 1;
	date_time->day = epoch - days[year][month] + 1;
}

/**
 *	@brief: get current year
 */
short Rtc::gety()
{
	epoch_to_date(&curdate, get());
	return 1970 + curdate.year;
}
char Rtc::getd()
{
	epoch_to_date(&curdate, get());
	return curdate.day;
}
char Rtc::getmn()
{
	epoch_to_date(&curdate, get());
	return curdate.month;
}
char Rtc::geth()
{
	epoch_to_date(&curdate, get());
	return curdate.hour + ZONE_CHARLIE;
}
char Rtc::getm()
{
	epoch_to_date(&curdate, get());
	return curdate.minute;
}
char Rtc::gets()
{
	epoch_to_date(&curdate, get());
	return curdate.second;
}
void Rtc::print(Console *output)
{
	output->xprintf("%04u/%02u/%02u %02u:%02u:%02u\n", \
			gety(), getmn(), getd(), geth(), getm(), gets());
}

word Rtc::state(void)
{
	return State;
}


