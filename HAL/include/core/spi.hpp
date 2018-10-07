#pragma once
#include <core/stm32f10x.hpp>
#include "../drivers/generic/iodriver.hpp"

#define SPI1NSS_PIN A,4,SPEED_50MHz
#define SPI1SCK_PIN A,5,SPEED_50MHz
#define SPI1MISO_PIN A,6,SPEED_50MHz
#define SPI1MOSI_PIN A,7,SPEED_50MHz

class Spi : public IODriver
{
public:
 Spi(char ch);
 ~Spi(void);

 virtual uint16_t read(uint16_t data = 0xFF) override;
 uint16_t read_no_nss(uint16_t data);
 virtual void multiread(uint8_t *buff, uint32_t size) override;
 virtual void multiwrite(const uint8_t *buff, uint32_t size) override;
 virtual void isr(uint32_t address) override;
 void lowspeed(void);
 void highspeed(void);
 void go8bit(void) ;
 void go16bit(void) ;
 void disable(void);
 void enable(void);
 void assert(void);
 void nss_low();
 void nss_hi();
 void signup() override;
 void signout() override;
 Spi *next;
protected:
 void init(void);
 int channel;
 SPI_TypeDef* Reg;
 Spi* extirq;
};


