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

#include <hal/adc.hpp>
#include <hal/io_macro.hpp>
#include <common.hpp>

namespace ADC
{
#define TEMP A,0,SPEED_10MHz
void Adc::init(void)
  {
    PIN_INPUT_ANALOG(TEMP);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CFGR |= RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV8;

    ADC1->CR2 |= ADC_CR2_CAL;
    while (!(ADC1->CR2 & ADC_CR2_CAL))
      ;

    ADC1->SQR2 |= ADC_SQR2_SQ12_0;

    ADC1->CR2 |= (ADC_CR2_EXTSEL_0 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2
        | ADC_CR2_EXTTRIG);
    ADC1->CR2 |= ADC_CR2_ADON;
    delay_ms(100);
  }

short Adc::sample(void)
  {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while (!(ADC1->SR & ADC_SR_EOC))
      ;
    short result = ADC1->DR & 0xFFFF;
    ADC1->SR = 0;
    return result;
  }

short Adc::voltage(void)
  {
    return sample() / 4096 * 3;
  }
}