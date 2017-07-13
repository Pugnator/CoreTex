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
 * 2015
 *******************************************************************************/

#include <drivers/console.hpp>
#include <stdarg.h>

#define DW_CHAR		sizeof(char)
#define DW_SHORT	sizeof(short)
#define DW_LONG		sizeof(long)

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void
Console::xputc (char c)
{
 if ( _CR_CRLF && c == '\n' )
  xputc ('\r'); /* CR -> CRLF */

 if ( outptr )
 {
  *outptr++ = (unsigned char) c;
  return;
 }

 print (c);
}

/*----------------------------------------------*/
/* Put a null-terminated string                 */
/*----------------------------------------------*/

void
Console::xputs ( /* Put a string to the default device */
const char* str /* Pointer to the string */
)
{
 while (*str)
  xputc (*str++);
}

/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);			"1234"
 xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
 xprintf("%-6u", 100);			"100   "
 xprintf("%ld", 12345678L);		"12345678"
 xprintf("%04x", 0xA3);			"00a3"
 xprintf("%08LX", 0x123ABC);		"00123ABC"
 xprintf("%016b", 0x550F);		"0101010100001111"
 xprintf("%s", "String");		"String"
 xprintf("%-4s", "abc");			"abc "
 xprintf("%4s", "abc");			" abc"
 xprintf("%c", 'a');				"a"
 xprintf("%f", 10.0);            <xprintf lacks floating point support>
 */

void
Console::xvprintf (const char* fmt, /* Pointer to the format string */
                   va_list arp /* Pointer to arguments */
                   )
{
 unsigned int r, i, j, w, f;
 unsigned long v;
 char s[16], c, d, *p;

 for (;;)
 {
  c = *fmt++; /* Get a char */
  if ( !c )
   break; /* End of format? */
  if ( c != '%' )
  { /* Pass through it if not a % sequense */
   xputc (c);
   continue;
  }
  f = 0;
  c = *fmt++; /* Get first char of the sequense */
  if ( c == '0' )
  { /* Flag: '0' padded */
   f = 1;
   c = *fmt++;
  }
  else
  {
   if ( c == '-' )
   { /* Flag: left justified */
    f = 2;
    c = *fmt++;
   }
  }
  for (w = 0; c >= '0' && c <= '9'; c = *fmt++) /* Minimum width */
   w = w * 10 + c - '0';
  if ( c == 'l' || c == 'L' )
  { /* Prefix: Size is long int */
   f |= 4;
   c = *fmt++;
  }
  if ( !c )
   break; /* End of format? */
  d = c;
  if ( d >= 'a' )
   d -= 0x20;
  switch (d)
  { /* Type is... */
   case 'S': /* String */
    p = va_arg(arp, char*);
    for (j = 0; p[j]; j++)
     ;
    while (!(f & 2) && j++ < w)
     xputc (' ');
    xputs (p);
    while (j++ < w)
     xputc (' ');
    continue;
   case 'C': /* Character */
    xputc((char)va_arg(arp, int)); continue;
   case 'B': /* Binary */
    r = 2;
    break;
   case 'O': /* Octal */
    r = 8;
    break;
   case 'D': /* Signed decimal */
   case 'U': /* Unsigned decimal */
    r = 10;
    break;
   case 'X': /* Hexdecimal */
    r = 16;
    break;
   default: /* Unknown type (passthrough) */
    xputc (c);
    continue;
  }

  /* Get an argument and put it in numeral */
  v = (f & 4) ? va_arg(arp, long) : ((d == 'D') ? (long)va_arg(arp, int) : (long)va_arg(arp, unsigned int));
  if ( d == 'D' && (v & 0x80000000) )
  {
   v = 0 - v;
   f |= 8;
  }
  i = 0;
  do
  {
   d = (char) (v % r);
   v /= r;
   if ( d > 9 )
    d += (c == 'x') ? 0x27 : 0x07;
   s[i++] = d + '0';
  }
  while (v && i < sizeof(s));
  if ( f & 8 )
   s[i++] = '-';
  j = i;
  d = (f & 1) ? '0' : ' ';
  while (!(f & 2) && j++ < w)
   xputc (d);
  do
   xputc (s[--i]);
  while (i);
  while (j++ < w)
   xputc (' ');
 }
}

void
Console::xprintf ( /* Put a formatted string to the default device */
const char* fmt, /* Pointer to the format string */
                  ... /* Optional arguments */
                  )
{
 va_list arp;
 va_start (arp, fmt);
 xvprintf (fmt, arp);
 va_end (arp);
}

void
Console::xsprintf ( /* Put a formatted string to the memory */
char* buff, /* Pointer to the output buffer */
                   const char* fmt, /* Pointer to the format string */
                   ... /* Optional arguments */
                   )
{
 va_list arp;

 outptr = buff; /* Switch destination for memory */

 va_start (arp, fmt);
 xvprintf (fmt, arp);
 va_end (arp);

 *outptr = 0; /* Terminate output string with a \0 */
 outptr = 0; /* Switch destination for device */
}

/*----------------------------------------------*/
/* Dump a line of binary dump                   */
/*----------------------------------------------*/

void
Console::put_dump (const void* buff, /* Pointer to the array to be dumped */
                   word addr, /* Heading address value */
                   word len, /* Number of items to be dumped */
                   word width /* Size of the items (DF_CHAR, DF_SHORT, DF_LONG) */
                   )
{
 const unsigned char *bp;
 const unsigned short *sp;
 const unsigned long *lp;

 xprintf ("%08lX\n", addr); /* address */

 switch (width)
 {
  case DW_CHAR:
   bp = (unsigned char*) buff;
   for (word i = 1; i < len + 1; i++) /* Hexdecimal dump */
    xprintf (" %02X%s", bp[i - 1], i % 16 ? "" : "\n");
   xputc ('\n');
   for (word i = 0; i < len; i++) /* ASCII dump */
    xputc ((bp[i] >= ' ' && bp[i] <= '~') ? bp[i] : '.');
   break;
  case DW_SHORT:
   sp = (short unsigned int*) buff;
   do /* Hexdecimal dump */
    xprintf (" %04X", *sp++);
   while (--len);
   break;
  case DW_LONG:
   lp = (long unsigned int*) buff;
   do /* Hexdecimal dump */
    xprintf (" %08LX", *lp++);
   while (--len);
   break;
 }

 xputc ('\n');
}
