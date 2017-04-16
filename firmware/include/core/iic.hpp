#pragma once
#include <core/stm32f10x.hpp>
#include "drivers/generic/iodriver.hpp"

class IIC : private IODriver
{
public:
 IIC(char ch);
 ~IIC(void);

 short read(short data);
 virtual void isr(void);

protected:
 int channel;
 I2C_TypeDef* Reg;
};

