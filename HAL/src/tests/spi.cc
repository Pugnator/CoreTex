#include <global.hpp>
#include <core/spi.hpp>
#include <stm32f10x_sdio.h>

#define RUN_TEST(x, y)   \
  do                     \
  {                      \
    result = x(y);       \
    if (false == result) \
      return false;      \
  } while (0)

bool spi_test()
{
  bool result = false;
  size_t test_value = 0xABCD;
  PrintF("*SPI test started\r\n");
  Spi s(1);
  size_t retval = 0;
  s.go8bit();
  s.lowspeed();
  PrintF("Entered low speed mode\r\n");
  PrintF("*Testing 8bit mode\r\n");
  retval = 0;
  retval = s.read(test_value);
  PrintF("Retval is 0x%X, should be 0x%X\r\n", retval, test_value & 0xFF);
  if (retval != (test_value & 0xFF))
  {
    return false;
  }
  s.go16bit();
  PrintF("*Testing 16bit mode\r\n");
  retval = 0;
  retval = s.read(test_value);
  PrintF("Retval is 0x%X, should be 0xABCD\r\n", retval, test_value & 0xFFFF);
  if (retval != (test_value & 0xFFFF))
  {
    return false;
  }
  PrintF("*Throughput test\r\n");
  s.highspeed();
  PrintF("Entered high speed mode\r\n");
  int bytes2send = 1024 * 1024;
  timerms = 1;
  s.go16bit();
  for (int i = 0; i < bytes2send / 2; ++i)
  {
    s.read(test_value);
  }
  size_t time = (timerms - 1) / 1000;
  PrintF("[Byte by byte] %u bytes transfered in %ums (%ub/s)\r\n", bytes2send, timerms, bytes2send / time);
  timerms = 0;

  size_t buf_size = 1024;
  uint8_t buf[buf_size];
  timerms = 1;
  for (int i = 0; i < bytes2send / 2; i += buf_size)
    s.multiwrite(buf, sizeof buf);

  time = (timerms - 1) / 1000;
  PrintF("[Burst mode] %u bytes transfered in %ums (%ub/s)\r\n", bytes2send, timerms, bytes2send / time);
  timerms = 0;
  PrintF("Test finished\r\n");
  return result;
}

bool tests_exec()
{
  bool result = false;
  USART out(1, CONSOLE_SPEED);
  Console con(&out);
  con.cursor(false);
  con.cls();
  con.print("STARTING CORE TESTS\r\n");
  RUN_TEST(spi_test, &con);
  return result;
}
