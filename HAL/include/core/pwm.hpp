#pragma once
#include <core/gpio.hpp>

namespace IO
{
class PWM_pin : public GPIO_pin
{
public:
  PWM_pin(PINCFG conf, PWM_pin *isrptr = nullptr);
  ~PWM_pin();

public:  
  
  void pwm_out_duty(uint16_t duty);

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