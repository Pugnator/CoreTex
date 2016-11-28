#pragma once
#include <core/stm32f10x.hpp>

class I2c
{
public:
 I2c(char ch);
 ~I2c(void);

 short read(short data);
 static class I2c *self;
 static void isr(void);

protected:
 int channel;
 I2C_TypeDef* Reg;
};

