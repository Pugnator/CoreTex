#pragma once
#include <core/stm32f10x.hpp>
#include <string.h>
#include <drivers/generic/driver.hpp>

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

    virtual void write (char c);
    virtual void writestr(const char* str);
    virtual char read(void);
    virtual const char* name();
    void dma_on (void);
    void dma_off (void);
    void dmatx_go(word size);
    void dmarx_go();
    void disable (void);
    static class Uart *self;
    /* ISRs */
    static void isr (void);
    static void dmarx (void);
    static void dmatx (void);
  protected:
    bool is_dma_on();
    uint8_t outbuf[32];
    uint8_t inbuf[32];
    void init (char channel, word baud);
    word channel;
    USART_TypeDef* Reg;
  };
}
