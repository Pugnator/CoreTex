#pragma once
#include <global.hpp>
#include <hal/stm32f10x.hpp>
namespace ADC
{
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
}
