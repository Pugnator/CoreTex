#include <xprintf.h>
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
	~GPX ()
	{
		commit();
	}

	bool create(const char* filename);
	bool commit(void);
	bool set_point(void);

private:
	FATFS filesystem;
	FRESULT result;
	FIL gpx;
	Gps *gps;
	unsigned written;
	word track_count;
};
