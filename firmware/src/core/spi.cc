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
#include <stdint.h>
#include <common.hpp>
#include <global.hpp>
#include <hal/io_macro.hpp>
#include <hal/stm32f10x.hpp>
#include <hal/spi.hpp>
#include <hal/isr_helper.hpp>

namespace SPI
{
class Spi *Spi::self = nullptr;

Spi::Spi(char ch)
  {
    __disable_irq();
    self = this;
    channel = ch;
    switch (channel)
      {
      case 1:
        Reg = (SPI_TypeDef*) SPI1_BASE;
        IRQ_VECTOR_TABLE[SPI1_IRQn + IRQ0_EX] = (word) &isr;
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
        PORT_ENABLE_CLOCK_START()
        /* enable all clock */
        PORT_ENABLE_CLOCK_ENTRY( SPI1SCK_PIN )
        PORT_ENABLE_CLOCK_ENTRY( SPI1MOSI_PIN )
        PORT_ENABLE_CLOCK_ENTRY( SPI1MISO_PIN )
        PORT_ENABLE_CLOCK_ENTRY( SPI1NSS_PIN )

        PORT_ENABLE_CLOCK_END()

        PIN_OUT_ALT_PP(SPI1SCK_PIN);
        PIN_OUT_ALT_PP(SPI1MOSI_PIN);
        PIN_OUT_PP(SPI1NSS_PIN);
        PIN_HI(SPI1NSS_PIN);
        PIN_INPUT_FLOATING(SPI1MISO_PIN);
        break;
      case 2:
        Reg = (SPI_TypeDef*) SPI2_BASE;
        IRQ_VECTOR_TABLE[SPI2_IRQn + IRQ0_EX] = (word) &isr;
        break;
      default:
        //ERROR
        ;
      }
    init();
    __enable_irq();
  }

Spi::~Spi(void)
  {
    __disable_irq();
    switch (channel)
      {
      case 1:
        IRQ_VECTOR_TABLE[SPI1_IRQn + IRQ0_EX] = (word) &SPI1_IRQHandler;
        break;
      case 2:
        IRQ_VECTOR_TABLE[SPI2_IRQn + IRQ0_EX] = (word) &SPI2_IRQHandler;
        break;
      default:
        //ERROR
        ;
      }
    __enable_irq();
    __ISB();
  }

void Spi::isr(void)
  {
    if (self->Reg->SR & SPI_SR_RXNE)
      {
        self->Reg->SR &= ~SPI_SR_RXNE;
      }
    else if ( SPI1->SR & SPI_SR_TXE)
      {
        self->Reg->SR &= ~SPI_SR_TXE;
      }
  }

void Spi::init(void)
  {
    RCC->APB2RSTR |= RCC_APB2ENR_SPI1EN;
    RCC->APB2RSTR &= ~RCC_APB2ENR_SPI1EN;
    disable();
    uint16_t tmp_cr1 = Reg->CR1;

      /* clear config bits */
      tmp_cr1 &= (~0x3040);

      /* 2 lines, full duplex, by default */
      tmp_cr1 |= 0;

      /* slave select to master */
      tmp_cr1 |= SPI_CR1_SSI | SPI_CR1_MSTR;

      /* 8bit mode by default */
      tmp_cr1 &= ~SPI_CR1_DFF;

      /* SPI MODE by default */
      /* SPI_CR1_CPHA | SPI_CR1_CPOL*/

      /* NSS control by software ( only used in slave mode ? ) */
      tmp_cr1 |= SPI_CR1_SSM;

      /* /32 */
      tmp_cr1 |= SPI_CR1_BR_2;

      Reg->CR1 = tmp_cr1;
      enable();
  }

uint16_t Spi::read(uint16_t data)
  {
    PIN_LOW(SPI1NSS_PIN);
    Reg->DR = data;
    while (Reg->SR & SPI_SR_BSY);
    uint16_t tmp = Reg->DR;
    PIN_HI(SPI1NSS_PIN);
    return tmp;
  }

uint16_t Spi::lazyread(uint16_t data)
  {
    Reg->DR = data;
    while (Reg->SR & SPI_SR_BSY);
    return Reg->DR;
  }

void Spi::go8bit(void)
  {
    //SPI module must be disabled
    disable();
    Reg->CR1 &= ~SPI_CR1_DFF;
    enable();
  }

void Spi::go16bit(void)
  {
    //SPI module must be disabled
    disable();
    Reg->CR1 |= SPI_CR1_DFF;
    enable();
  }

void Spi::disable(void)
  {
    Reg->CR1 &= ~SPI_CR1_SPE;
  }

void Spi::enable(void)
  {
    Reg->CR1 |= SPI_CR1_SPE;
  }

void Spi::low_speed(void)
  {
    Reg->CR1 &= ~SPI_CR1_BR;
    Reg->CR1 |= SPI_CR1_BR_2;
  }

void Spi::high_speed(void)
  {
    Reg->CR1 |= SPI_CR1_BR;
  }
}
