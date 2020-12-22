#pragma once
#include <drivers/console.hpp>
#include <config.hpp>
#include <SEGGER_RTT.h>
#include <memory>
#include <drivers/storage/ff.h>

class FATdisk;

#define LOGGING_CHANNEL 0

#define Print(x) SEGGER_RTT_WriteString(LOGGING_CHANNEL, x)
#define PrintF(...) SEGGER_RTT_printf(LOGGING_CHANNEL, ##__VA_ARGS__)

namespace LOG
{
class SDcardLog
{
  public:
  
  SDcardLog();
  
  bool open();
  void close();
  void write(const char* text);  

  std::unique_ptr<FATdisk> logdisk;
  FIL logf;
  FATFS fs;
  FRESULT fresult;
};

}