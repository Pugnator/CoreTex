#pragma once
#include <core/stm32f10x.hpp>
#include <core/gpio.hpp>
#include <memory>
#include <drivers/generic/iodriver.hpp>

class Spi : public IODriver
{
public:
 Spi(uint8_t ch);
 ~Spi(void);

 virtual uint16_t read(uint16_t data = 0xFF) override;
 uint16_t read_no_nss(uint16_t data);
 virtual void multiread(uint8_t *buff, uint32_t size) override;
 virtual void multiwrite(const uint8_t *buff, uint32_t size) override;
 virtual void isr(uint32_t address) override;
 void lowspeed(void);
 void highspeed(void);
 void go8bit(void);
 void go16bit(void);
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
 uint8_t channel;
 SPI_TypeDef* Reg;
 Spi* extirq;

 std::unique_ptr<IO::GPIO_pin> NSS_pin;
 std::unique_ptr<IO::GPIO_pin> SCK_pin;
 std::unique_ptr<IO::GPIO_pin> MISO_pin;
 std::unique_ptr<IO::GPIO_pin> MOSI_pin;
};

