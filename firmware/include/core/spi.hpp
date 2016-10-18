#pragma once
#include <core/stm32f10x.hpp>
#include <drivers/generic/driver.hpp>

namespace SPI
{
#define SPI1NSS_PIN A,4,SPEED_50MHz
#define SPI1SCK_PIN A,5,SPEED_50MHz
#define SPI1MISO_PIN A,6,SPEED_50MHz
#define SPI1MOSI_PIN A,7,SPEED_50MHz

class Spi : public Driver
  {
public:
  Spi(char ch);
  ~Spi(void);

  virtual uint16_t read(uint16_t data);
  virtual void multiread(uint8_t *buff, uint32_t size);
  virtual void multiwrite(const uint8_t *buff, uint32_t size);
  static class Spi *self;
  static void isr(void);
  void low_speed(void);
  void high_speed(void);
  void go8bit(void) ;
  void go16bit(void) ;
  void disable(void);
  void enable(void);
protected:
  void init(void);
  int channel;
  SPI_TypeDef* Reg;
  };
}

