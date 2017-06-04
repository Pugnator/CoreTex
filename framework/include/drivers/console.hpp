#pragma once
#ifdef __USE_CONSOLE
#include <global.hpp>
#include <core/usart.hpp>
#include <stdarg.h>

#define	_CR_CRLF 1	/* 1: Convert \n ==> \r\n in the output char */

enum
{
 COLOR_BLACK,
 COLOR_RED,
 COLOR_GREEN,
 COLOR_YELLOW,
 COLOR_BLUE,
 COLOR_MAGENTA,
 COLOR_CYAN,
 COLOR_WHITE
};

class Console
{
public:
 Console(class IODriver *_io)
{
  memset(conbuf, 0, sizeof conbuf);
  io = _io;
  self = this;
  outptr = nullptr;
  isdone = false;
}

 template<typename T>
 Console& operator<<(T arg)
 {
  print(arg);
  return *this;
 }
 ;
 template<typename T>
 Console& operator<(T arg)
 {
  print(arg);
  crlf();
  return *this;
 }
 ;

 void print(char ch);
 void print(char const* str);
 void print(int num);
 void print(word num);
 void crlf(void);

 void cls(void)
 {
  print("\x1B[2J\x1B[H"); //clear terminal
  print("\033[3J"); //PuTTY 0.59+, clears the real scroll back ;)
 }

 void cursor(bool state = true)
 {
  if (false == state)
  {
   print("\e[?25l");
  }
  else
  {
   print("\e[?25h");
  }
 }

 void foreground(char color = COLOR_WHITE);
 void background(char color = COLOR_BLACK);

 void error(char *message);

 void xputc(char c);
 void xputs(const char* str);
 void xprintf(const char* fmt, ...);
 void xsprintf(char* buff, const char* fmt, ...);
 void put_dump(const void* buff, word addr, word len, word width);
 void worker(void);

 bool isdone;
private:
 void xvprintf (const char*	fmt,va_list arp	);
 void log(char c);
 bool parse(void);
 char *outptr;
 char conbuf[24];
 char strbuf[8];
 static class Console *self;
 class IODriver *io;
};
#endif
