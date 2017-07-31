/*******************************************************************************
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 2015
 *******************************************************************************/
#include <common.hpp>
#include <log.hpp>
#include <core/io_macro.hpp>
#include <core/stm32f10x.hpp>
#include <global.hpp>
#include <xprintf.h>
#include <core/vmmu.hpp>
#include <core/rtc.hpp>
#include "gpx.hpp"

#ifdef GPX_DEBUG
#define DEBUG_LOG SEGGER_RTT_printf
#else
#define DEBUG_LOG(...)
#endif

static const char GPX_HEADER[] =
  "\
<?xml version=\"1.0\"?>\
<gpx>\
xmlns=\"http://www.topografix.com/GPX/1/1\"\
version=\"1.1\"\
creator=\"Tracker\"\
  <time>2017</time>\
    <metadata>\
      <name>STM32</name>\
      <desc>Sample track</desc>\
      <author>\
        <name>Pugnator</name>\
      </author>\
    </metadata>\
    <trk>\
      <name>Test run</name>\
      <trkseg>";

static const char GPX_TRACK_POINT[] =
  "\
        <trkpt lat=\"%d.%u\" lon=\"%d.%u\">\"\
          <time>%u</time>\
          <speed>%u</speed>\
          <msl>%d</msl>\
          <nmea>%s</nmea>\
    			<gsv>%u</gsv>\
        </trkpt>";

static const char GPX_FOOTER[] = "\
      </trkseg>\
    </trk>\
</gpx>";

#define MAX_WAYPOINT_PER_TRACK 10000

#define TRACKS_DIR "tracks"

namespace Tracker
{
using namespace GPS;
bool
GPX::create (word mode)
{
 track_count = 0;
 track_type = mode;
 result = mount (&filesystem, "0:", 1);
 DEBUG_LOG (0, "Disk result: %s\r\n", result_to_str (result));
 if ( FR_OK != result )
 {
  return false;
 }

 result = mkdir (TRACKS_DIR);
 if ( FR_OK != result && FR_EXIST != result )
  {
   DEBUG_LOG (0, "Failed to create dir %s\r\n", TRACKS_DIR);
   return false;
  }

 if ( FR_OK != chdir (TRACKS_DIR) )
 {
  DEBUG_LOG (0, "Failed to change to %s dir\r\n", TRACKS_DIR);
  return false;
 }

 char trackdir[8] =
 { 0 };
 Rtc r;
 xsprintf (trackdir, "%04u%02u%02u", r.gety(), r.getmn(), r.getd());

 result = mkdir (trackdir);
 if ( FR_OK != result && FR_EXIST != result )
 {
  DEBUG_LOG (0, "Failed to create dir %s\r\n", trackdir);
  return false;
 }

 if ( FR_OK != chdir (trackdir) )
 {
  DEBUG_LOG (0, "Failed to change to %s dir\r\n", trackdir);
  return false;
 }

 char trackname[8] =
 { 0 };
 xsprintf (trackname, "%u.gpx", ++track_count);

 DEBUG_LOG (0, "Creating track: %s/%s\r\n", trackdir, trackname);
 result = open (&gpx, trackname, FA_CREATE_ALWAYS | FA_WRITE);
 if ( result != FR_OK )
 {
  DEBUG_LOG (0, "Failed to open the file: %s\r\n",
                     result_to_str (result));
  return false;
 }
 unsigned written = 0;
 result = f_write (&gpx, GPX_HEADER, strlen (GPX_HEADER), &written);
 return FR_OK == result;
}

bool
GPX::new_dir ()
{
 if ( FR_OK != chdir ("..") )
 {
  return false;
 }

 mkdir ("tracks");
 return true;
}

bool
GPX::create ()
{
 char trackname[8] =
 { 0 };
 xsprintf (trackname, "%u.gpx", ++track_count);
 DEBUG_LOG (0, "Creating track: %s\r\n", trackname);
 result = open (&gpx, trackname, FA_CREATE_ALWAYS | FA_WRITE);
 if ( result != FR_OK )
 {
  DEBUG_LOG (0, "Failed to open the file: %s\r\n",
                     result_to_str (result));
  return false;
 }
 unsigned written = 0;
 result = f_write (&gpx, GPX_HEADER, strlen (GPX_HEADER), &written);
 return FR_OK == result;
}

bool
GPX::commit ()
{
 unsigned written = 0;
 result = f_write (&gpx, GPX_FOOTER, strlen (GPX_FOOTER), &written);
 close (&gpx);
 return FR_OK == result;
}

bool
GPX::set_point (void)
{
 if ( MAX_WAYPOINT_PER_TRACK == wpt_count )
 {
  wpt_count = 0;
  commit ();
  create ();
 }
 while (NMEA_ERROR_OK != gps->prepare ())
  ;
 if ( !gps->ok () )
 {
  gps->reset ();
  return false;
 }

 if ( 0 == track_type )
 {
  //unsigned written = 0;
  // return FR_OK == f_write (&gpx, gps->nmeastr, strlen (gps->nmeastr), &written);
 }

 coord lat = gps->getlat ();
 coord lon = gps->getlon ();
 UTM latutm = gps->coord2utm (lat);
 UTM lonutm = gps->coord2utm (lon);
 char *text = (char*) stalloc (256);
 xsprintf (text, GPX_TRACK_POINT, latutm.deg, latutm.fract, lonutm.deg,
           lonutm.fract, gps->get_utc(), gps->get_speed(), gps->get_alt(), gps->nmeastr, gps->gsv);

 DEBUG_LOG (0, "GPS: %s\r\n", text);
 unsigned written = 0;
 result = f_write (&gpx, text, strlen (text), &written);
 stfree (text);
 if ( result != FR_OK )
 {
  DEBUG_LOG (0, "Failed to write to the file: %s\r\n",
                     result_to_str (result));
  return false;
 }

 result = flush (&gpx);
 wpt_count++;
 gps->reset ();
 return FR_OK == result;
}
}
