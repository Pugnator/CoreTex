#pragma once
#include <core/stm32f10x.hpp>
#include "drivers/generic/iodriver.hpp"
#include <stdint.h>

class IIC : private IODriver
{
public:
 IIC(uint8_t ch);
 ~IIC(void);

 uint16_t read(uint16_t data);
 virtual void isr(void);

protected:
 uint8_t channel;
 I2C_TypeDef* Reg;
};

