#pragma once
#include "drivers/generic/iodriver.hpp"

namespace IO
{

 typedef enum
 {
	PORTA = 0, PORTB, PORTC, PORTD, PORTE
 }IOPORT;

class GPIO : public IODriver
{
public:
 GPIO(IOPORT _port);
 ~GPIO();
private:
 IOPORT port;
};
}
