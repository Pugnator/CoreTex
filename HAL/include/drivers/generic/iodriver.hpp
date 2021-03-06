#pragma once

#include <global.hpp>

class IODriver
{
public:
  IODriver()
  {
  }
  virtual ~IODriver()
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

  virtual char read(void)
  {
    return 0;
  }

  virtual void multiread(uint8_t *buff, uint32_t size)
  {
  }

  virtual void multiwrite(const uint8_t *buff, uint32_t size)
  {
  }

  virtual void signup()
  {
  }

  virtual void signout()
  {
  }

  virtual void isr(uint32_t address)
  {
  }
};
