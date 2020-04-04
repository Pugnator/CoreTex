#include <core/adc.hpp>
#include <core/isr_helper.hpp>
#include <log.hpp>

#define ADC_DEBUG
#ifdef ADC_DEBUG
#define DEBUG_LOG PrintF
#else
#define DEBUG_LOG(...)
#endif

namespace IO
{
ADC_pin::ADC_pin(PINCFG conf, ADC_pin *isrptr) : GPIO_pin(conf, isrptr)
{
  signup();
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  RCC->CFGR |= RCC_CFGR_ADCPRE;
  RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;
  ADC1->CR2 |= ADC_CR2_CAL;
  while (!(ADC1->CR2 & ADC_CR2_CAL))
    ;


  ADC1->SQR1 = ADC_SQR1_L_0;
  ADC1->SQR2 = 0;
  ADC1->SQR3 = 1; //ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_3;
  ADC1->SMPR1 = 0x4000000;
  ADC1->CR2 |= (ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG);
  ADC1->CR1 |= ADC_CR1_AWDCH_0 | ADC_CR1_AWDIE | ADC_CR1_AWDSGL | ADC_CR1_AWDEN;
  ADC1->HTR = 2100; //high threshold AWD
  ADC1->LTR = 2000; //low threshold AWD
  
  ADC1->CR2 |= ADC_CR2_ADON | ADC_CR2_CONT | ADC_CR2_SWSTART;  
  NVIC_EnableIRQ(ADC1_2_IRQn);
}

ADC_pin::~ADC_pin()
{
  signout();
}

uint16_t ADC_pin::adc_sample()
{
  ADC1->CR2 |= ADC_CR2_SWSTART;
  while (!(ADC1->SR & ADC_SR_EOC))
    ;
  uint16_t result = ADC1->DR & 0xFFFF;
  ADC1->SR = 0;
  return 0;
  return result;
}

double ADC_pin::adc_voltage()
{
  return adc_sample() / 4096 * 3;
}

void ADC_pin::signup()
{
  Reg = (ADC_TypeDef *)ADC1_BASE;

  ADC_pin *i = (ADC_pin *)HARDWARE_TABLE[ADC1_2_HANDLER];
  if (i)
  {
    DEBUG_LOG("Another instance of ADC is registered 0x%X, adding myself 0x%X\r\n", (uint32_t)i, (uint32_t)this);
    i->isr((uint32_t)this);
  }
  else
  {
    DEBUG_LOG("First ADC handler registration 0x%X\r\n", reinterpret_cast<uint32_t>(this));
    HARDWARE_TABLE[ADC1_2_HANDLER] = extirq ? reinterpret_cast<uint32_t>(extirq) : reinterpret_cast<uint32_t>(this);
  }
}

void ADC_pin::signout()
{
  HARDWARE_TABLE[ADC1_2_HANDLER] = next ? (uint32_t)next : 0;
}

void ADC_pin::isr(uint32_t address)
{
  if (address)
  {
    DEBUG_LOG("ADC_pin IRQ registration: 0x%X\r\n", address);
    next = reinterpret_cast<ADC_pin *>(address);
    return;
  }

  if (Reg->SR & ADC_SR_EOC)
  {
    uint16_t result = ADC1->DR & 0xFFFF;
    ADC1->SR = 0;
  }
}


/*===========================DMA===========================*/

ADC_DMA_pin::ADC_DMA_pin(PINCFG conf, ADC_DMA_pin *isrptr = nullptr) : GPIO_pin(conf, isrptr)
{
  signup();
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
  RCC->CFGR |= RCC_CFGR_ADCPRE;
  RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;

  ADC1->CR2 |= ADC_CR2_CAL;
  while (!(ADC1->CR2 & ADC_CR2_CAL))
    ;

  ADC1->SQR1 = ADC_SQR3_SQ1_0;
  ADC1->SQR2 = 0;
  ADC1->SQR3 = 1; //ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_3;
  ADC1->SMPR1 = 0x4000000;
  ADC1->CR2 |= (ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG);
  ADC1->CR1 = ADC_CR1_EOCIE; // Enable interrupt form End Of Conversion
  NVIC_EnableIRQ(ADC1_2_IRQn);
  ADC1->CR2 |= ADC_CR2_ADON | ADC_CR2_CONT | ADC_CR2_SWSTART;
}

ADC_DMA_pin::~ADC_DMA_pin()
{
  signout();
}

void ADC_DMA_pin::signup()
{
  Reg = (ADC_TypeDef *)ADC1_BASE;

  ADC_DMA_pin *i = (ADC_DMA_pin *)HARDWARE_TABLE[ADC1_2_HANDLER];
  if (i)
  {
    DEBUG_LOG("Another instance of ADC is registered 0x%X, adding myself 0x%X\r\n", (uint32_t)i, (uint32_t)this);
    i->isr((uint32_t)this);
  }
  else
  {
    DEBUG_LOG("First ADC handler registration 0x%X\r\n", reinterpret_cast<uint32_t>(this));
    HARDWARE_TABLE[ADC1_2_HANDLER] = extirq ? reinterpret_cast<uint32_t>(extirq) : reinterpret_cast<uint32_t>(this);
  }
}

void ADC_DMA_pin::signout()
{
  HARDWARE_TABLE[ADC1_2_HANDLER] = next ? (uint32_t)next : 0;
}

void ADC_DMA_pin::isr(uint32_t address)
{
  if (address)
  {
    DEBUG_LOG("ADC_DMA_pin IRQ registration: 0x%X\r\n", address);
    next = reinterpret_cast<ADC_DMA_pin *>(address);
    return;
  }

  if (Reg->SR & ADC_SR_EOC)
  {
    uint16_t result = ADC1->DR & 0xFFFF;
    ADC1->SR = 0;
  }
}
}