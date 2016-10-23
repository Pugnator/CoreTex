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
#include <strings.h>
#include <string.h>

extern "C"
{
int _getpid(void)
  {
    return 1;
  }

void _kill(int pid)
  {
  }

char*
_sbrk(int delta)
  {
    return nullptr;
  }

int _exit(void)
  {
    return 1;
  }

int _close(void)
  {
    return 1;
  }

int strcmp(const char* s1, const char* s2)
  {
    const unsigned char* c1 = (const unsigned char*) s1;
    const unsigned char* c2 = (const unsigned char*) s2;
    unsigned char ch;
    int d = 0;

    while (1)
      {
        d = (int) (ch = *c1++) - (int) *c2++;
        if (d || !ch) break;
      }

    return d;
  }

size_t strlen(const char* str)
  {
    register const char* s;

    for (s = str; *s; ++s)
      ;
    return (s - str);
  }

char*
strstr(const char* in, const char* str)
  {
    char c;
    size_t len;

    c = *str++;
    if (!c) return (char*) in; // Trivial empty string case

    len = strlen(str);
    do
      {
        char sc;

        do
          {
            sc = *in++;
            if (!sc) return (char*) 0;
          }
        while (sc != c);
      }
    while (strncmp(in, str, len) != 0);

    return (char*) (in - 1);
  }

int strncmp(const char* s1, const char* s2, size_t n)
  {
    for (; n > 0; s1++, s2++, --n)
      if (*s1 != *s2)
        return ((*(unsigned char*) s1 < *(unsigned char*) s2) ? -1 : +1);
      else if (*s1 == '\0') return 0;

    return 0;
  }

void*
memset(void* dst, int c, size_t n)
  {
    if (n)
      {
        char* d = (char*) dst;

        do
          {
            *d++ = c;
          }
        while (--n);
      }
    return dst;
  }

char*
strcpy(char* s1, const char* s2)
  {
    char* s = s1;
    while ((*s++ = *s2++) != 0)
      ;
    return (s1);
  }

char *
strcat(char *s1, const char *s2)
  {
    strcpy(&s1[strlen(s1)], s2);
    return s1;
  }

char*
strtok(char* s, const char* delim)
  {
    char* spanp;
    int c, sc;
    char* tok;
    static char* last;

    if (s == nullptr && (s = last) == nullptr) return (nullptr);

    /*
     * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
     */
    cont: c = *s++;
    for (spanp = (char*) delim; (sc = *spanp++) != 0;)
      {
        if (c == sc) goto cont;
      }

    if (c == 0) /* no non-delimiter characters */
      {
        last = nullptr;
        return (nullptr);
      }
    tok = s - 1;

    /*
     * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
     * Note that delim must have one NUL; we stop if we see that, too.
     */
    for (;;)
      {
        c = *s++;
        spanp = (char*) delim;
        do
          {
            if ((sc = *spanp++) == c)
              {
                if (c == 0)
                  s = nullptr;
                else
                  s[-1] = 0;
                last = s;
                return (tok);
              }
          }
        while (sc != 0);
      }
    /* NOTREACHED */
    return nullptr;
  }
}

int isdigit(int c)
  {
    return ((c >= '0') && (c <= '9'));
  }

double atof(char* s)
  {
    double a = 0.0;
    int e = 0;
    int c;
    while ((c = *s++) != '\0' && isdigit(c))
      {
        a = a * 10.0 + (c - '0');
      }
    if (c == '.')
      {
        while ((c = *s++) != '\0' && isdigit(c))
          {
            a = a * 10.0 + (c - '0');
            e = e - 1;
          }
      }
    if (c == 'e' || c == 'E')
      {
        int sign = 1;
        int i = 0;
        c = *s++;
        if (c == '+')
          c = *s++;
        else if (c == '-')
          {
            c = *s++;
            sign = -1;
          }
        while (isdigit(c))
          {
            i = i * 10 + (c - '0');
            c = *s++;
          }
        e += i * sign;
      }
    while (e > 0)
      {
        a *= 10.0;
        e--;
      }
    while (e < 0)
      {
        a *= 0.1;
        e++;
      }
    return a;
  }

void *
memcpy(void *dst, const void *src, size_t n)
  {
    const char *p = (char *) src;
    char *q = (char *) dst;
    while (n--)
      {
        *q++ = *p++;
      }
    return dst;
  }

int memcmp(const void *Ptr1, const void *Ptr2, size_t Count)
  {
    int v = 0;
    uint8_t *p1 = (uint8_t *) Ptr1;
    uint8_t *p2 = (uint8_t *) Ptr2;

    while (Count-- > 0 && v == 0)
      {
        v = *(p1++) - *(p2++);
      }

    return v;
  }

char *num2str(int i)
  {
    /* Room for INT_DIGITS digits, - and '\0' */
    char *buf = (char *)stalloc(12);
    char *p = buf + 10 + 1; /* points to terminating '\0' */
    if (i >= 0)
      {
        do
          {
            *--p = '0' + (i % 10);
            i /= 10;
          }
        while (i != 0);
        return p;
      }
    else
      { /* i < 0 */
        do
          {
            *--p = '0' - (i % 10);
            i /= 10;
          }
        while (i != 0);
        *--p = '-';
      }
    return p;
  }

void __wrap___aeabi_unwind_cpp_pr0()
{
	for(;;);
}

void* operator new(size_t n)
{
  void * const p = stalloc(n);
  // handle p == 0
  return p;
}

void operator delete(void * p) // or delete(void *, std::size_t)
{
	stfree(p);
}
