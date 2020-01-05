#include <drivers/l6470.hpp>
#include <log.hpp>

#define L6470_DEBUG 1

#ifdef L6470_DEBUG
#define DEBUG_LOG PrintF
#else
#define DEBUG_LOG(...)
#endif

/*
01A red
01B yellow
02A brown
02B blue
*/

l6470::l6470(short ch) : Spi(ch)
{
  PIN_OUT_PP(DSRST_PIN);
  PIN_LOW(DSRST_PIN);
  delay_ms(500);
  PIN_HI(DSRST_PIN);
  delay_ms(500);

  if (conncheck())
  {
    DEBUG_LOG("L6470 OK\r\n");
    setup();
  }
  else
  {
    DEBUG_LOG("L6470 communication failure\r\n");
    DEBUG_LOG("Ended\r\n");
  }
};

uint32_t l6470::calc(float stepsPerSec)
{
  //6250 steps per 1 deg
  // When issuing RUN command, the 20-bit speed is [(steps/s)*(tick)]/(2^-28) where tick is
  // 250ns (datasheet value).
  // Multiply desired steps/s by 67.106 to get an appropriate value for this register
  // This is a 20-bit value, so we need to make sure the value is at or below 0xFFFFF.

  float temp = stepsPerSec * 67.106;
  if ((uint32_t)int32_t(temp) > 0x000FFFFF)
  {
    return 0x000FFFFF;
  }
  else
  {
    return (uint32_t)temp;
  }
}

void l6470::go(float speed, uint8_t dir)
{
  read(dSPIN_RUN | dir);
  uint32_t spd = calc(speed);
  read(spd >> 16);
  read(spd >> 8);
  read(spd & 0xFF);
}

void l6470::stop(bool force)
{
  read(force ? dSPIN_HARD_STOP : dSPIN_SOFT_STOP);
}

void l6470::hiz(bool force)
{
  read(force ? dSPIN_HARD_HIZ : dSPIN_SOFT_HIZ);
}

void l6470::reset(bool force)
{
  if (force)
  {
    PIN_LOW(DSRST_PIN);
    delay_ms(500);
    PIN_HI(DSRST_PIN);
  }
  else
  {
    read(dSPIN_RESET_DEVICE);
  }
}

bool l6470::conncheck()
{
  read(dSPIN_GET_PARAM | 0x18);
  uint16_t config = (read() << 8) | read();
  DEBUG_LOG("config = 0x%hX\r\n", config);
  return config == 0x2E88;
}

void l6470::setup()
{
  read(dSPIN_SET_PARAM | 0x18);
  read(CONFIG_PWM_DIV_1 |
       CONFIG_PWM_MUL_2 |
       CONFIG_SR_290V_us |
       CONFIG_OC_SD_DISABLE |
       CONFIG_VS_COMP_DISABLE |
       CONFIG_SW_HARD_STOP |
       CONFIG_INT_16MHZ);

  delay_ms(500);
  uint8_t p = 0xFF;
  read(dSPIN_SET_PARAM | 0x09);
  read(p);
  delay_ms(500);
  read(dSPIN_SET_PARAM | 0x0A);
  read(p);
  delay_ms(500);
  read(dSPIN_SET_PARAM | 0x0B);
  read(p);
  delay_ms(500);
  read(dSPIN_SET_PARAM | 0x0C);
  read(p);
  delay_ms(500);

  uint8_t OCValue = (uint8_t)floor(375 / 375);
  read(dSPIN_SET_PARAM | 0x13);
  read(OCValue < 0x0F ? OCValue : 0x0F);
}
