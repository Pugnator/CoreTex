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
 * 2015, 2016
 *******************************************************************************/
#include <drivers/gps.hpp>
#include <common.hpp>

/*
 $GPAAM - Waypoint Arrival Alarm
 $GPALM - GPS Almanac Data
 $GPAPA - Autopilot Sentence "A"
 $GPAPB - Autopilot Sentence "B"
 $GPASD - Autopilot System Data
 $GPBEC - Bearing & Distance to Waypoint, Dead Reckoning
 $GPBOD - Bearing, Origin to Destination
 $GPBWC - Bearing & Distance to Waypoint, Great Circle
 $GPBWR - Bearing & Distance to Waypoint, Rhumb Line
 $GPBWW - Bearing, Waypoint to Waypoint
 $GPDBT - Depth Below Transducer
 $GPDCN - Decca Position
 $GPDPT - Depth
 $GPFSI - Frequency Set Information
 $GPGGA - Global Positioning System Fix Data
 $GPGLC - Geographic Position, Loran-C
 $GPGLL - Geographic Position, Latitude/Longitude
 $GPGSA - GPS DOP and Active Satellites
 $GPGSV - GPS Satellites in View
 $GPGXA - TRANSIT Position
 $GPHDG - Heading, Deviation & Variation
 $GPHDT - Heading, True
 $GPHSC - Heading Steering Command
 $GPLCD - Loran-C Signal Data
 $GPMTA - Air Temperature (to be phased out)
 $GPMTW - Water Temperature
 $GPMWD - Wind Direction
 $GPMWV - Wind Speed and Angle
 $GPOLN - Omega Lane Numbers
 $GPOSD - Own Ship Data
 $GPR00 - Waypoint active route (not standard)
 $GPRMA - Recommended Minimum Specific Loran-C Data
 $GPRMB - Recommended Minimum Navigation Information
 $GPRMC - Recommended Minimum Specific GPS/TRANSIT Data
 $GPROT - Rate of Turn
 $GPRPM - Revolutions
 $GPRSA - Rudder Sensor Angle
 $GPRSD - RADAR System Data
 $GPRTE - Routes
 $GPSFI - Scanning Frequency Information
 $GPSTN - Multiple Data ID
 $GPTRF - Transit Fix Data
 $GPTTM - Tracked Target Message
 $GPVBW - Dual Ground/Water Speed
 $GPVDR - Set and Drift
 $GPVHW - Water Speed and Heading
 $GPVLW - Distance Traveled through the Water
 $GPVPW - Speed, Measured Parallel to Wind
 $GPVTG - Track Made Good and Ground Speed
 $GPWCV - Waypoint Closure Velocity
 $GPWNC - Distance, Waypoint to Waypoint
 $GPWPL - Waypoint Location
 $GPXDR - Transducer Measurements
 $GPXTE - Cross-Track Error, Measured
 $GPXTR - Cross-Track Error, Dead Reckoning
 $GPZDA - Time & Date
 $GPZFO - UTC & Time from Origin Waypoint
 $GPZTG - UTC & Time to Destination Waypoint
 */

namespace GPS
{
NMEAERR
Gps::parse (char c)
{
 if ( nmea.nmeaerr && '$' != c )
 {
  return NMEA_NOT_BEGINNING;
 }
 switch (c)
 {
  case '$':
   *nmea.fp++ = c;
   break;
  case '*':
   nmea.sumdone = true;
   nmea.sect++;
   memset (nmea.fstr, 0, sizeof nmea.fstr);
   nmea.fp = nmea.fstr;
   break;
  case '\n':
   break;
  case '\r':
   nmea.nmeaok = ckecknmea (nmea.checksum, nmea.fstr);
   break;
  case ',':
   nmea.checksum ^= c;
   if ( 0 == nmea.sect )
   {
    if ( PMTK == get_nmea_talker (nmea.fstr) )
    {
     return NMEA_DIAGNOSTIC_MSG;
    }
    type = get_nmea_sent_type (nmea.fstr);
   }
   //TODO: make some kind of specialized template here based on NMEATYPE
   if ( *nmea.fstr )
   {
    switch (type)
    {
     case GGA:
      fillGGActx (nmea.sect, nmea.fstr);
      break;
     case VTG:
      fillVTGctx (nmea.sect, nmea.fstr);
      break;
     case RMC:
      fillRMCctx (nmea.sect, nmea.fstr);
      break;
     case GLL:
      fillGLLctx (nmea.sect, nmea.fstr);
      break;
     case GSV:
      fillGSVctx (nmea.sect, nmea.fstr);
      break;
     default:
      return NMEA_UNKNOWN_TALKER;
      break;
    }
   }
   nmea.sect++;
   memset (nmea.fstr, 0, sizeof nmea.fstr);
   nmea.fp = nmea.fstr;
   break;
  default:
   *nmea.fp++ = c;
   if ( !nmea.sumdone )
   {
    nmea.checksum ^= c;
   }
 }
 return NMEA_ERROR_OK;
}
}
