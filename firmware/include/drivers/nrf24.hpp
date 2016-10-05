#pragma once
#include <global.hpp>
#include <hal/spi.hpp>
#include <drivers/console.hpp>

using namespace HAL;
using namespace SPI;

namespace NRF24
{

#define NRFCE A,8,SPEED_50MHz

/* Registers */

typedef enum REGISTER
  {
  CONFIG,
  EN_AA,
  EN_RXADDR,
  SETUP_AW,
  SETUP_RETR,
  RF_CH,
  RF_SETUP,
  STATUS,
  OBSERVE_TX,
  RPD,
  RX_ADDR_P0,
  RX_ADDR_P1,
  RX_ADDR_P2,
  RX_ADDR_P3,
  RX_ADDR_P4,
  RX_ADDR_P5,
  TX_ADDR,
  RX_PW_P0,
  RX_PW_P1,
  RX_PW_P2,
  RX_PW_P3,
  RX_PW_P4,
  RX_PW_P5,
  FIFO_STATUS,
  DYNPD,
  FEATURE,
  } REGISTER;

typedef enum CMD
  {
  REGISTER_MASK = 0x1F,
  R_REGISTER = 0x00,
  W_REGISTER = 0x20,
  R_RX_PAYLOAD = 0x61,
  W_TX_PAYLOAD = 0xA0,
  FLUSH_TX = 0xE1,
  FLUSH_RX = 0xE2,
  REUSE_TX_PL = 0xE3,
  ACTIVATE = 0x50,
  R_RX_PL_WID = 0x60,
  W_ACK_PAYLOAD = 0xA8,
  W_TX_PAYLOAD_NO_ACK = 0xB0,
  NOP = 0xFF,
  } CMD;

class Nrf24: public Spi
  {
public:
  Nrf24(char ch)
      : Spi::Spi(ch)
    {
      PIN_OUT_PP(NRFCE);
      nrfinit();
    }

  uint8_t status(void);
  //single byte
  void regw(REGISTER reg, uint8_t value);
  //multibyte
  void regw(REGISTER reg, uint8_t *in, word size);
  //single byte
  uint8_t regr(REGISTER reg);
  //multibyte
  void regr(REGISTER reg, uint8_t *out, word size);
  //chip init
  void nrfinit();
  void debug(CONSOLE::Console *conout);
  void flushtx(void);
  void flushrx(void);

protected:
  };
}
