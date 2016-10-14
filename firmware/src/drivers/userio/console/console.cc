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

#include <global.hpp>
#include <common.hpp>
#include <hal/usart.hpp>
#include <hal/isr_helper.hpp>
#include <drivers/console.hpp>
#include <hal/io_macro.hpp>
#include <hal/vmmu.hpp>
#include <hal/rtc.hpp>
#include <queue.hpp>
#include <limits.h>

class Console *Console::self = nullptr;

namespace
{
  static void
  ver_func (Console *self);
  static void
  help_func (Console *self);
  static void
  reset_func (Console *self);
  static void
  dihalt_func (Console *self);
  static void
  date_func (Console *self);
  static void
  color_func (Console *self);
  static void
  cls_func (Console *self);
  static void
  fault_func (Console *self);
  static void
  led_func (Console *self);
  static void
  free_func (Console *self);
  static void
  memdump_func (Console *self);
  static void
  exit_func (Console *self);

  typedef struct command
  {
    const char* cmd;
    void
    (*funcptr) (Console *self);
  } command;

  static char *arg1 = nullptr;
  static char *arg2 = nullptr;
  void
  (*cmdptr) (Console *self);

  static const command cmd_list[] =
    {
      { "ver", &ver_func },
      { "cls", &cls_func },
      { "free", &free_func },
      { "reset", &reset_func },
      { "dihalt", &dihalt_func },
      { "date", &date_func },
      { "color", &color_func },
      { "help", &help_func },
      { "led", &led_func },
      { "dump", &memdump_func },
      { "fatal", &fault_func },
      { "exit", &exit_func },
      { nullptr, nullptr } };

  /************************************************************************/

#define CHECK_ARGS1 do{if(!arg1){stfree(arg1); return;} }while(0)
#define CHECK_ARGS2 do{if(!arg1 || !arg2){stfree(arg1); stfree(arg2); return;} }while(0)

  static void
  led_func (Console *self)
  {
    CHECK_ARGS1;
    if (!strcmp (arg1, "on"))
      {
	PIN_HI(LED);
      }
    else if (!strcmp (arg1, "off"))
      {
	PIN_LOW(LED);
      }
    else
      {
	self->xputs ("wrong parameter\r\n");
      }
    FREE(arg1);
  }

  static void
  memdump_func (Console *self)
  {
    CHECK_ARGS2;
    word address = str16_to_word (arg1);
    word len = str10_to_word (arg2);
    self->put_dump ((const void*) address, address, len, 1);
    FREE(arg1);
    FREE(arg2);
  }

  static void
  ver_func (Console *self)
  {
    self->print ("Command line parser for STM32F10X\r\n");
  }

  static void
  exit_func (Console *self)
  {
    self->isdone = true;
  }

  static void
  free_func (Console *self)
  {
    print_memstat (self);
  }

//!!!!!!!!!!!!! WE DO KNOW WHAT WE'RE DOING HERE!!!!!!!!!!!
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdiv-by-zero"
  static void
  fault_func (Console *self)
  {
    volatile word a = 10 / 0;
  }
#pragma GCC diagnostic pop
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  static void
  help_func (Console *self)
  {
    self->foreground (COLOR_RED);
    self->print ("Built-in commands:\r\n");
    self->foreground (COLOR_GREEN);
    for (int i = 0; cmd_list[i].cmd; ++i)
      {
	self->print (cmd_list[i].cmd);
	self->crlf ();
      }
    //TODO: implement get_foreground() end restore user selection
    self->foreground (COLOR_WHITE);
  }

  static void
  reset_func (Console *self)
  {
    (void) self;
    NVIC_SystemReset ();
  }

  static void
  dihalt_func (Console *self)
  {
    (void) self;
    self->foreground (COLOR_RED);
    self->print ("Entering sleep, interrupts off\r\n");
    __disable_irq ();
    __WFI ();
  }

  static void
  color_func (Console *self)
  {
    if (arg1)
      {
	self->foreground (str10_to_word (arg1));
	FREE(arg1);
      }

    if (arg2)
      {
	self->background (str10_to_word (arg2));
	FREE(arg2);
      }
  }

  static void
  cls_func (Console *self)
  {
    self->cls ();
  }

  static void
  date_func (Console *self)
  {
    Rtc rtc;
    if (arg1)
      {
	rtc.set (str10_to_word (arg1));
	FREE(arg1);
      }
    rtc.print (self);
  }
}
/*******************************************************************/

void
Console::worker ()
{
  char res = 0;
  while(!isdone)
    {
      if (0 == QueueGet (&res))
	{
	  log (res);
	}
    }
}
void
Console::log (char c)
{
  switch (c)
    {
    case '\r':
      crlf ();
      if (!*conbuf)
	{
	  break;
	}
      else if (!parse ())
	{
	  xprintf ("command not found [%s]\r\n", conbuf);
	}
      memset (conbuf, 0, sizeof conbuf);
      break;
    case 0x7F:
      print (c);
      conbuf[strlen (conbuf) - 1] = 0;
      break;
    default:
      {
	if (strlen (conbuf) + 1 >= sizeof conbuf)
	  break;
	//I'm too lazy
	char str[2] =
	  { 0 };
	str[0] = c;
	strcat (conbuf, str);
	print (c);
      }
    }
}

bool
Console::parse ()
{
  bool result = false;
  const char delim[2] = " ";
  char *token;
  token = strtok (conbuf, delim);
  for (int count = 0; token != NULL; ++count)
    {
      if (0 == count)
	{
	  //check for a command here, should be the 1st
	  for (int i = 0; cmd_list[i].cmd; ++i)
	    {
	      if (0 == strcmp (cmd_list[i].cmd, token))
		{
		  cmdptr = cmd_list[i].funcptr;
		  result = true;
		}
	    }
	  if (!result)
	    break;
	}
      else if (1 == count)
	{
	  arg1 = (char *) stalloc (strlen (token) + 1);
	  strcpy (arg1, token);
	}
      else if (2 == count)
	{
	  arg2 = (char *) stalloc (strlen (token) + 1);
	  strcpy (arg2, token);
	}
      token = strtok (NULL, delim);
    }
  if (result)
    {
      cmdptr (self);
    }
  return result;
}

void
Console::error (char * message)
{
  //FIXME: make it work :)
  //print("\x1b[31");
  print (message);
  //print("\x1b[0m");
}

void
Console::foreground (char color)
{
  //skip the warning about narrowing in a clean way
  char fattr = '0' + color;
  char forecolor[6] = "\x1b[30m";
  forecolor[3] = fattr;
  print (forecolor);
}

void
Console::background (char color)
{
  //skip the warning about narrowing in a clean way
  char fattr = '0' + color;
  char forecolor[6] = "\x1b[40m";
  forecolor[3] = fattr;
  print (forecolor);
}

void
Console::print (char ch)
{
  io->write (ch);
}

void
Console::print (char const* str)
{
  io->writestr (str);
}

void
Console::print (int num)
{
  int sign;
  memset (strbuf, 0, sizeof strbuf);
  if ((sign = num) < 0)
    {
      num = -num;
    }
  int i = 0;
  do
    {
      strbuf[i++] = num % 10 + '0';
    }
  while ((num /= 10) > 0);
  if (sign < 0)
    {
      strbuf[i++] = '-';
    }
  strbuf[i] = '\0';
  int right = strlen (strbuf) - 1;
  int left = 0;
  while (left < right)
    {
      strbuf[right] ^= strbuf[left];
      strbuf[left] ^= strbuf[right];
      strbuf[right] ^= strbuf[left];
      ++left;
      --right;
    }
  print (strbuf);
}

void
Console::print (word num)
{
  memset (strbuf, 0, sizeof strbuf);
  word i = 0;
  do
    {
      strbuf[i++] = num % 10 + '0';
    }
  while ((num /= 10) > 0);
  strbuf[i] = '\0';
  int right = strlen (strbuf) - 1;
  int left = 0;
  while (left < right)
    {
      strbuf[right] ^= strbuf[left];
      strbuf[left] ^= strbuf[right];
      strbuf[right] ^= strbuf[left];
      ++left;
      --right;
    }
  print (strbuf);
}

void
Console::crlf (void)
{
  print ("\r\n");
}
