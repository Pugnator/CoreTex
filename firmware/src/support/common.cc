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
#include <core/vmmu.hpp>
#include <global.hpp>
#include <stdint.h>
#include <string.h>

static uint32_t seed=2463534242;

word str16_to_word(const char* str)
{
	word res = 0;
	char c = 0;
	while (*str)
	{
		c = *str;
		res <<= 4;
		res += (c > '9') ? (c & 0xDFu) - 0x37u : (c - '0');
		++str;
	}
	return res;
}

word str10_to_word(const char* str)
{
	word res = 0;
	while (*str)
	{
		if ((*str >= '0') && (*str <= '9'))
		{
			res = (res * 10) + ((*str) - '0');
		}
		else if (',' == *str || '.' == *str)
		{
			break;
		}
		str++;
	}
	return res;
}

void delay_ms(word ms)
{
	tickcounter = ms;
	while (tickcounter);
}

void delay(word s)
{
	delay_ms(s * 1000);
}

void _delay_us(word us)
{

}

char *strclone(const char *msg)
{
        char *str = (char *)ALLOC(strlen(msg)+1);
        if(!str)
          return nullptr;
        strcpy(str,msg);
        return str;
}

char *ucs2ascii (const char *ucs2)
  {
    word size = strlen(ucs2);
    char *ascii = (char*)ALLOC(size+1);
    if(!ascii)
      return nullptr;
    memset(ascii, 0, size+1);
    char *p = ascii;
    char s[3]={0};
    for(word i=2; i<size; i+=4)
      {
        s[0] = ucs2[i];
        s[1] = ucs2[i+1];
        *p++ = str16_to_word((const char*)&s);
      }
    return ascii;
  }

void ascii2ucs2( const char *ascii )
{
  const char *ptr = ascii;
  while( *ptr )
  {
   /* if( *ptr < 127 ) modem2u16( *ptr );
    else if ( *ptr == 168 ) modem2u16( 0x0401 );
    else if ( *ptr == 184 ) modem2u16( 0x0451 );
    else modem2u16( (*ptr-192+1040) );*/
    ++ptr;
  }
}

uint32_t xorshift()
{
  seed ^= (seed << 13);
  seed ^= (seed >> 17);
  seed ^= (seed << 5);
  return seed;
}