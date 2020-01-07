#pragma once
#include <core/gpio.hpp>

namespace IO
{
class PWM_pin : public GPIO_pin
{
public:
  ADC_pin(PINCFG conf, PWM_pin *isrptr = nullptr);
  ~ADC_pin();

public:
  /* ADC */
  void adc(bool enable);
  void adc_setup();
  uint16_t adc_sample();
  double adc_voltage();

  PWM_pin *next;
  virtual void isr(uint32_t address);

private:
  void signup();
  void signout();

protected:  
  PINCFG config;
  PWM_pin *extirq;
};
}