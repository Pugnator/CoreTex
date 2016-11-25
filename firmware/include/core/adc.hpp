#pragma once
#include <core/stm32f10x.hpp>
#include <global.hpp>

class Adc
{
public:
 Adc(void){init();};
 ~Adc(void){};
 short sample(void);
 short voltage(void);

protected:
 void init(void);
 int channel;
 ADC_TypeDef* Reg;
};

