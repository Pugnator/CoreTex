#pragma once

namespace mcu
{
class Mcu
{
public:
	Mcu();
	~Mcu();
	void sleep ( void );
private:
};
}


void delay_ms ( int ms );
void delay ( int s );
