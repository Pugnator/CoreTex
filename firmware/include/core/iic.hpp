#pragma once
#include <core/stm32f10x.hpp>

class IIC
{
public:
 IIC(char ch);
 ~IIC(void);

 short read(short data);
 static class IIC *self;
 static void isr(void);

protected:
 int channel;
 I2C_TypeDef* Reg;
};

