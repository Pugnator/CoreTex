/*******************************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 2015
 *******************************************************************************/

#pragma GCC diagnostic ignored "-Wswitch"
#include <core/gpio.hpp>
#include <common.hpp>
#include <log.hpp>
#include <core/isr_helper.hpp>

namespace IO
{

 GPIO_pin::GPIO_pin(PINCFG conf)
 {
	config = conf;
	Reg = nullptr;
	pwm_enabled = false;
	adc_enabled = false;
	//Let headers to configure it according to MCU type by itself
	switch (conf.port)
	{
	 case PORTA:
		pbase = GPIOA;
		break;
	 case PORTB:
		pbase = GPIOB;
		break;
	 case PORTC:
		pbase = GPIOC;
		break;
	 case PORTD:
		pbase = GPIOD;
		break;
	 case PORTE:
		pbase = GPIOE;
		break;
	 default:
		Print("Wrong port provided\n");
		__assert(!conf.port, __FILE__, __LINE__);
		break;
	}

	((uint32_t*) pbase)[config.index >> 3u] &= ~(0x0Fu << ((config.index % 8u) << 2u));
	switch (config.mode)
	{
	 case IN_PD:
		((uint32_t*) pbase)[config.index >> 3u] |= (0x08u | 0) << ((config.index % 8u) << 2u);
		pbase->BRR = (1u << config.index);
		break;
	 case IN_PU:
		((uint32_t*) pbase)[config.index >> 3u] |= (0x08u | 0) << ((config.index % 8u) << 2u);
		pbase->BSRR = (1u << config.index);
		break;
	 case IN_FLT:
		((uint32_t*) pbase)[config.index >> 3u] |= (0x04u | config.speed) << ((config.index % 8u) << 2u);
		break;
	 case IN_ANALOG:
		((uint32_t*) pbase)[config.index >> 3u] |= (0 | config.speed) << ((config.index % 8u) << 2u);
		break;
	 case OUT_PP:
		((uint32_t*) pbase)[config.index >> 3u] |= (0x00u | config.speed) << ((config.index % 8u) << 2u);
		break;
	 case OUT_OD:
		((uint32_t*) pbase)[config.index >> 3u] |= (0x04u | config.speed) << ((config.index % 8u) << 2u);
		break;
	 case OUT_ALT_PP:
		((uint32_t*) pbase)[config.index >> 3u] |= (0x08u | config.speed) << ((config.index % 8u) << 2u);
		break;
	 case OUT_ALT_OD:
		((uint32_t*) pbase)[config.index >> 3u] |= (0x0Cu | config.speed) << ((config.index % 8u) << 2u);
		break;
	 default:
		Print("Wrong pin mode provided\n");
		__assert(!conf.port, __FILE__, __LINE__);
		break;
	}

	HARDWARE_TABLE[EXTI0_HANDLER] = (uint32_t) this;
	//int irqnum = USART1_IRQn - 1;
	//NVIC_EnableIRQ((IRQn_Type) irqnum);
	//NVIC_SetPriority((IRQn_Type) irqnum, 3);
 }

 GPIO_pin::~GPIO_pin()
 {
 }

 void GPIO_pin::isr(uint32_t address)
 {

 }

 void GPIO_pin::low()
 {
	pbase->BRR = (1u << config.index);
 }

 void GPIO_pin::hi()
 {
	pbase->BSRR = (1u << config.index);
 }

 void GPIO_pin::toggle()
 {
	pbase->BSRR =
	  (pbase->ODR & (1u << config.index)) ? ((1u << config.index) << 16u) : (1u << config.index);
 }

 void GPIO_pin::pwm(bool enable)
 {
	pwm_enabled = enable;
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

	 if (enable)
	 {
		TIM2->CR1 |= TIM_CR1_CEN;
	 }
	 else
	 {
		TIM2->CR1 &= ~TIM_CR1_CEN;
	 }
	}
	else if (GPIOB == pbase)
	{

	}
 }

 void GPIO_pin::pwm_invert(bool invert)
 {
	if (!pwm_enabled)
	{
	 pwm(true);
	}
 }

 void GPIO_pin::pwm_duty(uint16_t duty)
 {
	if (!pwm_enabled)
	{
	 pwm(true);
	}
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

 void GPIO_pin::adc(bool enable)
 {
	adc_enabled = enable;
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->CFGR |= RCC_CFGR_ADCPRE;
	RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;

	ADC1->CR2 |= ADC_CR2_CAL;
	while (!(ADC1->CR2 & ADC_CR2_CAL))
	 ;

	ADC1->SQR1 = 0;
	ADC1->SQR2 = 0;
	ADC1->SQR3 = ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_3;

	ADC1->CR2 |= (ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTTRIG);
	ADC1->CR2 |= ADC_CR2_ADON;
 }

 uint16_t GPIO_pin::adc_sample()
 {
	if (!adc_enabled)
	{
	 adc(true);
	}
	ADC1->CR2 |= ADC_CR2_SWSTART;
	while (!(ADC1->SR & ADC_SR_EOC))
	 ;
	uint16_t result = ADC1->DR & 0xFFFF;
	ADC1->SR = 0;
	return result;
 }

 double GPIO_pin::adc_voltage()
 {
	return adc_sample() / 4096 * 3;
 }

 PINSTATE GPIO_pin::get_state()
 {
	return pbase->IDR & (1u << config.index) ? SET : RESET;
 }

 GPIO::GPIO(IOPORT _port)
 {
	port = _port;
 }

 GPIO::~GPIO()
 {
 }
}
