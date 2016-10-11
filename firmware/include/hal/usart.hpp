#pragma once
#include <string.h>
#include <hal/stm32f10x.hpp>
#include <drivers/generic/driver.hpp>

namespace HAL
{
namespace UART
{

#define RX1 A,10,SPEED_50MHz
#define TX1 A,9,SPEED_50MHz
#define RX2 A,3,SPEED_50MHz
#define TX2 A,2,SPEED_50MHz
#define RX3 B,11,SPEED_50MHz
#define TX3 B,10,SPEED_50MHz

  class Uart : public Driver
  {
  public:
    Uart (word ch, word bd);
    Uart (word ch, word bd, void (*isrptr)(void));
    ~Uart(void);

    void write (char c);
    void writestr(const char* str);
    char read(void);
    const char* name();
    void disable (void);
    static class Uart *self;
    static void isr (void);
  protected:
    void init (char channel, word baud);
    word channel;
    USART_TypeDef* Reg;
  };
}
}
