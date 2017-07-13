#pragma once
#include <core/stm32f10x.hpp>
#include <global.hpp>

class Adc
{
public:
 Adc (void)
 {
  init ();
 }
 ;
 ~Adc (void)
 {
 }
 ;
 uint16_t
 sample (void);
 uint16_t
 voltage (void);

protected:
 void
 init (void);
 int channel;
 ADC_TypeDef* Reg;
};
