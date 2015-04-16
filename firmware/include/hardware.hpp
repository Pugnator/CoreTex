#pragma once
#include <global.hpp>

namespace mcu
{
class MCU
{
public:
	MCU ( void );
	~MCU ( void );
	HANDLE* open ( char const* path );
	void close ( HANDLE* hnd );
private:
};
}
