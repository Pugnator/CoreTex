#pragma once
#include <global.hpp>
#include <../res/text/drivers/text.hpp>

namespace HAL
{
class Driver
  {
public:
  Driver()
    {

    }
  virtual ~Driver()
    {

    }
  virtual void write(char byte)
    {

    }
  virtual void writen(char byte, uint16_t size)
    {

    }
  virtual void writestr(const char *str)
    {

    }
  virtual uint16_t read(uint16_t data)
    {
      return 0;
    }
  virtual const char* name(void)
    {
      return GENERIC_DRIVER_DISPLAY_NAME;
    }
  };
}