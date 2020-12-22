#pragma once

/**
 * @brief [brief description]
 * @details [long description]
 * @return [description]
 */
class timer
{
public:
  timer(void);
  void on(void);
  void off(void);
  void reset(void);

private:
  int channel;
};

