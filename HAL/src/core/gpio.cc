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

#include <core/gpio.hpp>
#include <common.hpp>
#include <log.hpp>
#include <core/stm32f10x.hpp>
#include <core/isr_helper.hpp>

#define GPIO_DEBUG
#ifdef GPIO_DEBUG
#define DEBUG_LOG PrintF
#else
#define DEBUG_LOG(...)
#endif

namespace IO
{

GPIO_pin::GPIO_pin(PINCFG conf, GPIO_pin *isrptr)
{
  extirq = isrptr;
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

  ((uint32_t *)pbase)[config.index >> 3u] &= ~(0x0Fu << ((config.index % 8u) << 2u));
  switch (config.mode)
  {
  case IN_PD:
    ((uint32_t *)pbase)[config.index >> 3u] |= (0x08u | 0) << ((config.index % 8u) << 2u);
    pbase->BRR = (1u << config.index);
    break;
  case IN_PU:
    ((uint32_t *)pbase)[config.index >> 3u] |= (0x08u | 0) << ((config.index % 8u) << 2u);
    pbase->BSRR = (1u << config.index);
    break;
  case IN_FLT:
    ((uint32_t *)pbase)[config.index >> 3u] |= (0x04u | config.speed) << ((config.index % 8u) << 2u);
    break;
  case IN_ANALOG:
    ((uint32_t *)pbase)[config.index >> 3u] |= (0 | config.speed) << ((config.index % 8u) << 2u);
    break;
  case OUT_PP:
    ((uint32_t *)pbase)[config.index >> 3u] |= (0x00u | config.speed) << ((config.index % 8u) << 2u);
    break;
  case OUT_OD:
    ((uint32_t *)pbase)[config.index >> 3u] |= (0x04u | config.speed) << ((config.index % 8u) << 2u);
    break;
  case OUT_ALT_PP:
    ((uint32_t *)pbase)[config.index >> 3u] |= (0x08u | config.speed) << ((config.index % 8u) << 2u);
    break;
  case OUT_ALT_OD:
    ((uint32_t *)pbase)[config.index >> 3u] |= (0x0Cu | config.speed) << ((config.index % 8u) << 2u);
    break;
  default:
    Print("Wrong pin mode provided\n");
    __assert(!conf.port, __FILE__, __LINE__);
    break;
  }

  //HARDWARE_TABLE[EXTI0_HANDLER] = (uint32_t)this;
  //int irqnum = USART1_IRQn - 1;
  //NVIC_EnableIRQ((IRQn_Type) irqnum);
  //NVIC_SetPriority((IRQn_Type) irqnum, 3);
}

GPIO_pin::~GPIO_pin()
{
  
}

void GPIO_pin::isr(uint32_t address)
{
  if (address)
  {
    DEBUG_LOG("GPIO_pin IRQ registration: 0x%X\r\n", address);
    next = reinterpret_cast<GPIO_pin *>(address);
    return;
  }

  if (Reg->SR & ADC_SR_EOC)
  {
    uint16_t result = ADC1->DR & 0xFFFF;
    ADC1->SR = 0;
  }
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
} // namespace IO
