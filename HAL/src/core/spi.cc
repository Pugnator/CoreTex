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
#include <log.hpp>
#include <core/io_macro.hpp>
#include <core/isr_helper.hpp>
#include <core/spi.hpp>
#include <core/stm32f10x.hpp>
#include <global.hpp>

#ifdef SPI_DEBUG
#define DEBUG_LOG PrintF
#else
#define DEBUG_LOG(...)
#endif

Spi::Spi(char ch)
{
 DEBUG_LOG(0,"SPI%u activated\r\n", ch);
 __disable_irq();
 channel = ch;
 next = nullptr;
 switch (channel)
 {
	case 1:
	 Reg = (SPI_TypeDef*) SPI1_BASE;
	 RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	 PORT_ENABLE_CLOCK_START()
	 /* enable all clock */
	 PORT_ENABLE_CLOCK_ENTRY(SPI1SCK_PIN)PORT_ENABLE_CLOCK_ENTRY(SPI1MOSI_PIN)PORT_ENABLE_CLOCK_ENTRY(SPI1MISO_PIN)PORT_ENABLE_CLOCK_ENTRY(SPI1NSS_PIN)

	 PORT_ENABLE_CLOCK_END()

	 PIN_OUT_ALT_PP(SPI1SCK_PIN);
	 PIN_OUT_ALT_PP(SPI1MOSI_PIN);
	 PIN_OUT_PP(SPI1NSS_PIN);
	 PIN_HI(SPI1NSS_PIN);
	 PIN_INPUT_FLOATING(SPI1MISO_PIN);
	 break;
	case 2:
	 Reg = (SPI_TypeDef*) SPI2_BASE;
	 break;
	default:
	 //ERROR
	 ;
 }
 signup();
 init();
 __enable_irq();
}

Spi::~Spi(void)
{
 __disable_irq();
 switch (channel)
 {
	case 1:
	 HARDWARE_TABLE[SPI1_IRQn + IRQ0_EX] = (uint32_t) this;
	 break;
	case 2:
	 HARDWARE_TABLE[SPI2_IRQn + IRQ0_EX] = (uint32_t) this;
	 break;
	default:
	 //ERROR
	 ;
 }
 __enable_irq();
 __ISB();
}

void Spi::isr(uint32_t address)
{
 if (address)
 {
	PrintF("Spi IRQ registration: 0x%X\r\n", address);
	next = (Spi*) address;
	return;
 }

 if (Reg->SR & SPI_SR_RXNE)
 {
	Reg->SR &= ~SPI_SR_RXNE;
	uint16_t c = Reg->DR;
 }

 if (Reg->SR & SPI_SR_TXE)
 {
	Reg->SR &= ~SPI_SR_TXE;
 }

 if (Reg->SR & SPI_SR_UDR)
 {
	Reg->SR &= ~SPI_SR_UDR;
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

 /* NSS control by software */
 tmp_cr1 |= SPI_CR1_SSM;

 /* /32 */
 tmp_cr1 |= SPI_CR1_BR_2;

 Reg->CR1 = tmp_cr1;

 uint16_t tmp_cr2 = 0;
 tmp_cr2 |= SPI_CR2_RXNEIE | SPI_CR2_ERRIE;
 Reg->CR2 = tmp_cr2;

 //NVIC_EnableIRQ (SPI1_IRQn);
 //NVIC_SetPriority(SPI1_IRQn, 3);

 enable();
}

void Spi::nss_hi()
{
 PIN_HI(SPI1NSS_PIN);
}

void Spi::nss_low()
{
 PIN_LOW(SPI1NSS_PIN);
}

uint16_t Spi::read(uint16_t data)
{
 PIN_LOW(SPI1NSS_PIN);
 while (!(Reg->SR & SPI_SR_TXE))
	;
 Reg->DR = data;
 while (!(Reg->SR & SPI_SR_RXNE))
	;
 volatile uint16_t tmp = Reg->DR;
 PIN_HI(SPI1NSS_PIN);
 return tmp;
}

uint16_t Spi::read_no_nss(uint16_t data)
{
 while (!(Reg->SR & SPI_SR_TXE))
	;
 Reg->DR = data;
 while (!(Reg->SR & SPI_SR_RXNE))
	;
 volatile uint16_t tmp = Reg->DR;
 return tmp;
}

void Spi::multiread(uint8_t *buf, uint32_t size)
{
 if (1 == size)
 {
	go8bit();
	PIN_LOW(SPI1NSS_PIN);
	Reg->DR = 0xFF;
	while (Reg->SR & SPI_SR_BSY)
	 ;
	buf[0] = Reg->DR;
	PIN_HI(SPI1NSS_PIN);
	return;
 }

 go16bit();
 volatile uint16_t dr = 0;
 PIN_LOW(SPI1NSS_PIN);
 for (uint32_t i = 0; i < size / 2; ++i)
 {
	Reg->DR = 0xFFFF;
	while (Reg->SR & SPI_SR_BSY)
	 ;
	dr = Reg->DR;
	buf[1] = dr & 0xFF;
	buf[0] = dr >> 8;
	buf += 2;
 }
 PIN_HI(SPI1NSS_PIN);
}

void Spi::multiwrite(const uint8_t *buf, uint32_t size)
{
 if (1 >= size || size % 2)
	return;

 go16bit();
 PIN_LOW(SPI1NSS_PIN);
 for (uint32_t i = 0; i < size / 2; ++i)
 {
	Reg->DR = buf[0] << 8 | buf[1];
	while (Reg->SR & SPI_SR_BSY)
	 ;
	volatile uint16_t dr = Reg->DR;
	buf += 2;
 }
 PIN_HI(SPI1NSS_PIN);
}

void Spi::go8bit(void)
{
 DEBUG_LOG(0,"Spi::go8bit\r\n");
 //SPI module must be disabled
 disable();
 Reg->CR1 &= ~SPI_CR1_DFF;
 enable();
}

void Spi::go16bit(void)
{
 DEBUG_LOG(0,"Spi::go16bit\r\n");
 //SPI module must be disabled
 disable();
 Reg->CR1 |= SPI_CR1_DFF;
 enable();
}

void Spi::disable(void)
{
 DEBUG_LOG(0,"Spi::disable\r\n");
 Reg->CR1 &= ~SPI_CR1_SPE;
}

void Spi::enable(void)
{
 DEBUG_LOG(0,"Spi::enable\r\n");
 Reg->CR1 |= SPI_CR1_SPE;
}

void Spi::assert(void)
{
 DEBUG_LOG(0,"Spi::assert\r\n");
 PIN_HI(SPI1NSS_PIN);
 delay_ms(100);
 PIN_LOW(SPI1NSS_PIN);
 PIN_HI(SPI1NSS_PIN);
}

void Spi::lowspeed(void)
{
 DEBUG_LOG("Spi::lowspeed\r\n");
 Reg->CR1 &= ~SPI_CR1_BR;
 Reg->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2; // \256
}

void Spi::highspeed(void)
{
 DEBUG_LOG("Spi::highspeed\r\n");
 Reg->CR1 &= ~SPI_CR1_BR;

}

void Spi::signup()
{
 switch (channel)
 {
	case 1:
	 Reg = (SPI_TypeDef*) SPI1_BASE;
	 break;
	case 2:
	 Reg = (SPI_TypeDef*) SPI2_BASE;
	 break;
	case 3:
	 Reg = (SPI_TypeDef*) SPI3_BASE;
	 break;
	default:
	 DEBUG_LOG("Unsupported Spi channel\r\n");
	 return;
 }

 Spi* i = (Spi*) HARDWARE_TABLE[SPI1_HANDLER];
 if (i)
 {
	DEBUG_LOG("Another instance of Spi is registered 0x%X, adding myself 0x%X\r\n", (uint32_t)i,(uint32_t)this);
	i->isr((uint32_t) this);
 }
 else
 {
	DEBUG_LOG("First Spi handler registration 0x%X\r\n", (uint32_t)this);
	HARDWARE_TABLE[SPI1_HANDLER] = (uint32_t) this;
	DEBUG_LOG("Handler address 0x%X\r\n", (uint32_t)HARDWARE_TABLE[SPI1_HANDLER]);
 }
}

void Spi::signout()
{
 HARDWARE_TABLE[SPI1_HANDLER] = next ? (uint32_t) next : 0;
}

