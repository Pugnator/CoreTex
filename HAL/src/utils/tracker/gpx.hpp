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
	 track_count(0),
	 wpt_count(0),
	 track_type(0)
  {

  }
	~GPX ()
	{
		commit();
	}

	bool create(const char* filename, word mode);
	bool commit(void);
	bool set_point(void);

private:
	FATFS filesystem;
	FRESULT result;
	FIL gpx;
	Gps *gps;
	word track_count;
	word wpt_count;
	word track_type;
};
