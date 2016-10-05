#pragma once
#include <hal/stm32f10x.hpp>
#include <drivers/generic/driver.hpp>
namespace HAL
{
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

  uint16_t read(uint16_t data);
  uint16_t lazyread(uint16_t data);
  static class Spi *self;
  static void isr(void);
protected:
  void low_speed(void);
  void high_speed(void);
  void go8bit(void) ;
  void go16bit(void) ;
  void disable(void);
  void enable(void);
  void init(void);
  word error;
  int channel;
  SPI_TypeDef* Reg;
  };
}
}
