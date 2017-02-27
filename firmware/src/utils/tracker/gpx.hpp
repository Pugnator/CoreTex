#include <xprintf.h>
#include <xml/xml.hpp>
#include <drivers/storage/fatdisk.hpp>
#include <drivers/gps.hpp>

class GPX : public FATdisk
{
public:
	GPX (char *filename) : FATdisk (1)
{

}
};
