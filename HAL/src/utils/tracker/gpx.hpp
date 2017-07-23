#include <xprintf.h>
#include <drivers/storage/fatdisk.hpp>
#include <drivers/gps.hpp>

namespace Tracker
{
class GPX : public FATdisk
{
public:
 GPX (Gps* g) :
   FATdisk (1), filesystem (
   { 0 }), result (FR_OK), gpx (
   { 0 }), gps (g), wpt_count (0), track_type (0), track_count (0)
 {

 }
 ~GPX ()
 {
  commit ();
 }

 bool
 create (word mode);
 bool
 create (void);
 bool
 commit (void);
 bool
 set_point (void);

private:
 bool
 new_dir ();
private:
 FATFS filesystem;
 FRESULT result;
 FIL gpx;
 Gps *gps;
 word wpt_count;
 word track_type;
 word track_count;
};
}
