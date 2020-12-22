#include <core/pwm.hpp>
#include <core/isr_helper.hpp>
#include <log.hpp>

#pragma GCC diagnostic ignored "-Wswitch"

#define PWM_DEBUG
#ifdef PWM_DEBUG
#define DEBUG_LOG PrintF
#else
#define DEBUG_LOG(...)
#endif

namespace IO
{
PWM_pin::PWM_pin(PINCFG conf, PWM_pin *isrptr) : GPIO_pin(conf, isrptr)
{  
  if (GPIOA == pbase)
  {
    switch (config.index)
    {
    case P0:
      RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
      TIM2->CCER |= TIM_CCER_CC1E; //enable pin
      TIM2->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;
      TIM2->CCMR1 &= ~TIM_CCMR1_OC1M_0;
      break;
    case P1:
      RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
      TIM2->CCER |= TIM_CCER_CC2E; //enable pin
      TIM2->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
      TIM2->CCMR1 &= ~TIM_CCMR1_OC2M_0;
      break;
    case P2:
      RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
      TIM2->CCER |= TIM_CCER_CC3E; //enable pin
      TIM2->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
      TIM2->CCMR2 &= ~TIM_CCMR2_OC3M_0;
      break;
    case P3:
      RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
      TIM2->CCER |= TIM_CCER_CC4E; //enable pin
      TIM2->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;
      TIM2->CCMR2 &= ~TIM_CCMR2_OC4M_0;
      break;
    }

    TIM2->CR1 |= TIM_CR1_CEN;
  }
  else if (GPIOB == pbase)
  {
  }
}

PWM_pin::~PWM_pin()
{
  TIM2->CR1 &= ~TIM_CR1_CEN;
}

void PWM_pin::pwm_out_duty(uint16_t duty)
{
  if (GPIOA == pbase)
  {
    switch (config.index)
    {
    case P0:
    case P1:
    case P2:
    case P3:
      TIM2->CCR2 = (0xFFFF / 100) * duty;
      break;
    }
  }
}
}