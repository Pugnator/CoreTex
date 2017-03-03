#include <xprintf.h>
#include <xml/xml.hpp>
#include <drivers/storage/fatdisk.hpp>
#include <drivers/gps.hpp>

class GPX : public FATdisk
{
public:
	GPX (Gps* g) :
	 FATdisk (1),
	 filesystem ({0}),
	 result (FR_OK),
	 gpx ({0}),
	 gps (g),
	 written(0),
	 track_count(0)
  {

  }

	bool create(const char* filename);
	bool commit(void);
	bool do_point(void);

private:
	FATFS filesystem;
	FRESULT result;
	FIL gpx;
	Gps *gps;
	unsigned written;
	word track_count;
};
