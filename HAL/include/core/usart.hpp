#pragma once
#include <core/stm32f10x.hpp>
#include <string.h>

#include "drivers/generic/iodriver.hpp"

#define RX1 A,10,SPEED_50MHz
#define TX1 A,9,SPEED_50MHz
#define RX2 A,3,SPEED_50MHz
#define TX2 A,2,SPEED_50MHz
#define RX3 B,11,SPEED_50MHz
#define TX3 B,10,SPEED_50MHz

class USART : private IODriver
{
public:
 USART (uint32_t ch, uint32_t bd, USART *isrptr = nullptr);
 ~USART(void);

 virtual void write (char c) override;
 virtual void writestr(const char* str) override;
 virtual char read(void) override;
 virtual const char* name() override; 
 void disable (void);
 USART* next;
 /* ISRs */
 virtual void isr (uint32_t address) override; 
 uint8_t *get_rx_buf();
 uint8_t *get_tx_buf();
protected:
 void signup() override;
 void signout() override;
 uint8_t outbuf[32];
 uint8_t inbuf[32];
 void init (char channel, uint32_t baud);
 uint32_t channel;
 USART_TypeDef* Reg;
 USART* extirq;
};
