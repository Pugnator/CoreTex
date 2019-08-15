#pragma once
#include "drivers/generic/iodriver.hpp"
#include <iterator>

namespace IO
{

  /* PIN */

  typedef enum
  {
    PORTA = 0, PORTB, PORTC, PORTD, PORTE
  } IOPORT;

  typedef enum
  {
    P0 = 0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
  } IOPIN;

  typedef enum
  {
    IOSPEED_10MHz = 1, IOSPEED_20MHz, IOSPEED_50MHz
  } IOSPEED;

  typedef enum
  {
    IN_PD = 0, IN_PU, IN_FLT, IN_ANALOG, OUT_PP, OUT_OD, OUT_ALT_PP, OUT_ALT_OD
  } IOMODE;

  typedef enum
  {
    RESET = 0, SET, FLOATING
  } PINSTATE;

  typedef struct
  {
    IOPORT port;
    IOPIN index;
    IOSPEED speed;
    IOMODE mode;
  } PINCFG;

  class GPIO_pin : public IODriver
  {
  public:
    GPIO_pin (PINCFG conf);
    ~GPIO_pin ();

  public:
    void toggle ();
    void low ();
    void hi ();
    /* PWM */
    void pwm(bool enable);
    void pwm_invert(bool invert);
    void pwm_duty(uint16_t duty);
    /* ADC */
    void adc(bool enable);
    uint16_t adc_sample();
    double adc_voltage();

    PINSTATE get_state ();
    virtual void isr(uint32_t address);
  public:
    PINCFG get_config ();
  protected:
    bool pwm_enabled;
    bool adc_enabled;
    EXTI_TypeDef* Reg;
    PINCFG config;
    GPIO_TypeDef* pbase;
  };

  /* PORT */

  class GPIO : public IODriver
  {
  public:
    GPIO (IOPORT _port);
    ~GPIO ();

  public:
    void start ();
    void stop ();
  private:
    IOPORT port;
  };

  /* PORT iterator */

  template<typename T>
    class gpio_iterator : public std::iterator<std::forward_iterator_tag, T>
    {
      typedef gpio_iterator<T> iterator;
      GPIO_pin pin;
    public:
      gpio_iterator () :
          pin (nullptr)
      {
      }
      ~gpio_iterator ()
      {
      }
    };

  template<typename T>
    gpio_iterator<T>
    begin (T *val)
    {
      return gpio_iterator<T> (val);
    }

  template<typename T, typename Tsize>
    gpio_iterator<T>
    end (T *val, Tsize size)
    {
      return gpio_iterator<T> (val) + size;
    }

}
