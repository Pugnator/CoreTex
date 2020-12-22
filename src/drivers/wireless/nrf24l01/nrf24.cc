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
#include <global.hpp>
#include <drivers/nrf24.hpp>
#include <drivers/console.hpp>

uint8_t Nrf24::status(void)
{
  return read(NOP);
}
void Nrf24::regw(REGISTER reg, uint8_t value)
{
  (void)read(W_REGISTER | (REGISTER_MASK & reg));
  (void)read(value);
}

void Nrf24::regw(REGISTER reg, uint8_t *in, uint32_t size)
{
  (void)read(W_REGISTER | (REGISTER_MASK & reg));
  uint8_t *p = in;
  for (uint32_t i = 0; i < size; ++i)
  {
    read(*p++);
  }
}

void Nrf24::regr(REGISTER reg, uint8_t *out, uint32_t size)
{
  (void)read(W_REGISTER | (REGISTER_MASK & reg));
  uint8_t *p = out;
  for (uint32_t i = 0; i < size; ++i)
  {
    *p++ = read(0xff);
  }
}

uint8_t Nrf24::regr(REGISTER reg)
{
  read(R_REGISTER | (REGISTER_MASK & reg));
  return read(NOP);
}

void Nrf24::nrfinit(void)
{
  PIN_LOW(NRFCE);
  delay_ms(250);
  (void)read(ACTIVATE | 0x73);
  delay_ms(250);
  regw(CONFIG, 0x8);
  regw(EN_AA, 0x3f);
  regw(EN_RXADDR, 0x3);
  regw(SETUP_AW, 0x3);
  regw(SETUP_RETR, 0x3);
  regw(RF_CH, 0x2);
  regw(RF_SETUP, 0xE);
  regw(STATUS, 0x70);
  regw(RX_ADDR_P0, (uint8_t *)"00000", 5);
  regw(RX_ADDR_P1, (uint8_t *)"11111", 5);
  regw(RX_ADDR_P2, '2');
  regw(RX_ADDR_P3, '3');
  regw(RX_ADDR_P4, '4');
  regw(RX_ADDR_P5, '5');
  regw(TX_ADDR, (uint8_t *)"00000", 5);
  regw(RX_PW_P0, 0);
  regw(RX_PW_P1, 0);
  regw(RX_PW_P2, 0);
  regw(RX_PW_P3, 0);
  regw(RX_PW_P4, 0);
  regw(RX_PW_P5, 0);

  regw(DYNPD, 0);
  regw(FEATURE, 0x1);

  flushtx();
  flushrx();
}

void Nrf24::flushtx(void)
{
  (void)read(FLUSH_TX);
}

void Nrf24::flushrx(void)
{
  (void)read(FLUSH_RX);
}

void Nrf24::debug(void)
{
  uint8_t status = read(STATUS);
}
