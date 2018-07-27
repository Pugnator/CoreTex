#include <drivers/l6470.hpp>
/*
01A red
01B yellow
02A brown
02B blue
*/
uint32_t l6470::calc(float stepsPerSec)
{
 //6250 steps per 1 deg
 // When issuing RUN command, the 20-bit speed is [(steps/s)*(tick)]/(2^-28) where tick is
 // 250ns (datasheet value).
 // Multiply desired steps/s by 67.106 to get an appropriate value for this register
 // This is a 20-bit value, so we need to make sure the value is at or below 0xFFFFF.

 float temp = stepsPerSec * 67.106;
 if( (uint32_t) int32_t(temp) > 0x000FFFFF)
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
