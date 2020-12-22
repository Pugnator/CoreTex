#pragma once
#include <core/gpio.hpp>

namespace IO
{
class ADC_pin : public GPIO_pin
{
public:
  ADC_pin(PINCFG conf, ADC_pin *isrptr = nullptr);
  ~ADC_pin();

public:
  uint16_t adc_sample();
  double adc_voltage();

  ADC_pin *next;
  virtual void isr(uint32_t address);

private:
  void signup();
  void signout();

protected:
  ADC_TypeDef *Reg;
};

class ADC_DMA_pin : public GPIO_pin
{
public:
  ADC_DMA_pin(PINCFG conf, ADC_DMA_pin *isrptr = nullptr);
  ~ADC_DMA_pin();

public:

  ADC_DMA_pin *next;
  virtual void isr(uint32_t address);

private:
  void signup();
  void signout();

protected:
  ADC_TypeDef *Reg;
};
}