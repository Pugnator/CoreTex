#pragma once

namespace logger
{
class Log
{
public:
	Log();
	~Log();
	void print ( char const* msg );
private:
	uart::Uart* debug;
};

}