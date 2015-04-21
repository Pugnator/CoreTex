#pragma once
#include <global.hpp>

void delay_ms ( int ms );
void delay ( int s );

namespace Common
{
	class Error
	{
	public:
		Error();
		bool status;
		char getlast();
		char next();
		void operator+ (char c);
		char operator-- (void);
	private:
		uart::Stack errors;
	};


}
