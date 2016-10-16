#include <core/spi.hpp>
#include <global.hpp>
#include <drivers/console.hpp>

namespace
{
#define RUN_TEST(x) do\
{\
	result = x();\
	if(false == result) return false;\
}while(0)


bool spi_test()
{
	bool result = false;
	return result;
}
}

bool run_tests()
{
	bool result = false;
	RUN_TEST(spi_test);
	return result;
}
