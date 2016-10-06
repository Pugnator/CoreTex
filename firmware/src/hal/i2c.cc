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

#include <hal/i2c.hpp>
#include <hal/io_macro.hpp>
#include <global.hpp>
#include <common.hpp>
#include <hal/stm32f10x.hpp>

namespace HAL
{
namespace I2C
{
#define I2C1SDA_PIN B,7,SPEED_50MHz
#define I2C1SCK_PIN B,6,SPEED_50MHz
I2c::I2c(char ch)
  {
    channel = ch;
    Reg = (I2C_TypeDef*) nullptr;
    PIN_OUT_ALT_OD(I2C1SCK_PIN);
    PIN_OUT_ALT_OD(I2C1SDA_PIN);
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
    /* Enable I2C1 reset state */
    RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST;
    /* Release I2C1 from reset state */
    RCC->APB1RSTR &= ~RCC_APB1RSTR_I2C1RST;
    I2C1->CR1 |= I2C_CR1_PE;
    I2C1->DR = 0xAA;
    //I2C1->TIMINGR = (((word)0x40912732) & ((word)0xF0FFFFFF));

    //I2C1->CR2 = (1 << 16) | I2C_CR2_START | 0xA0;

  }
}
}

