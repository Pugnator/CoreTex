#pragma once
#include "drivers/generic/iodriver.hpp"

namespace IO
{

 typedef enum
 {
	PORTA = 0, PORTB, PORTC, PORTD, PORTE
 } IOPORT;

 typedef enum
 {
	P0 = 0, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12,
 } IOPIN;

 typedef enum
 {
	IOSPEED_10MHz = 0, IOSPEED_20MHz, IOSPEED_50MHz
 } IOSPEED;

 typedef enum
 {
	IN_PD = 0, IN_PU, IN_FLT, IN_ANALOG, OUT_PP, OUT_OD, OUT_ALT_PP, OUT_ALT_OD
 } IOMODE;

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
	void start();
	void stop();
	PINCFG get_config ();
 private:
	PINCFG config;
 };

 class GPIO : public IODriver
 {
 public:
	GPIO (IOPORT _port);
	~GPIO ();

 public:
 private:
	IOPORT port;
 };
}
