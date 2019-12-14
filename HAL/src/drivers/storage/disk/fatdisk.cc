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

#include <drivers/storage/fatdisk.hpp>
#include <drivers/storage/ffconf.h>
#include <drivers/storage/integer.hpp>
#include <log.hpp>
#include <stdint.h>
#include <config.hpp>

#ifdef FATDISK_DEBUG
#define DEBUG_LOG SEGGER_RTT_printf
#else
#define DEBUG_LOG(...)
#endif

FATdisk::FATdisk (uint8_t channel)
: Sdc(channel)
{
 SDCstat = 0;
}

DWORD FATdisk::get_fattime (void)
{
 return 0;
}

DSTATUS
FATdisk::disk_initialize (BYTE drv)
{
 DEBUG_LOG(0,"disk_initialize: Drive=%u\r\n", drv);
 lowspeed();
 go8bit();
 if (drv)
 {
  DEBUG_LOG(0,"disk_initialize: STA_NOINIT\r\n");
  return STA_NOINIT; /* Supports only drive 0 */
 }

 if (SDCstat & STA_NODISK)
 {
  DEBUG_LOG(0,"disk_initialize: MMCstat=%u\r\n", SDCstat);
  return SDCstat; /* Is card existing in the soket? */
 }

 if( SD_RESPONSE_FAILURE == initialize())
 {
  DEBUG_LOG(0,"SD init failed\r\n");
  SDCstat = STA_NOINIT;
 }
 else
 {
  DEBUG_LOG(0,"SD init OK\r\n");
  highspeed();
  SDCstat &= ~STA_NOINIT;
 }
 return SDCstat;
}

DSTATUS
FATdisk::disk_status (BYTE drv)
{
 return drv ? STA_NOINIT : SDCstat;
}

DRESULT
FATdisk::disk_read (BYTE drv, BYTE* buff, DWORD sector, UINT count)
{
 /* LBA ==> BA for SDv1*/
 if (!isSDCv2)
 {
  sector*=FF_MIN_SS;
 }

 count*=FF_MIN_SS;

 DEBUG_LOG(0,"disk_read: Drive=%u, sector=%u, count=%u\r\n", drv, sector, count);
 if(SD_RESPONSE_NO_ERROR == read_block(buff, sector, count ))
 {
  return RES_OK; /* Return result */
 }
 else
 {
  return RES_ERROR; /* Return result */
 }
}

DRESULT
FATdisk::disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
 /* LBA ==> BA conversion (byte addressing cards) */
 if (!isSDCv2)
 {
  sector*=FF_MIN_SS;
 }

 count*=FF_MIN_SS;
 DEBUG_LOG(0,"disk_write: Drive=%u, sector=%u, count=%u\r\n", drv, sector, count);
 if(SD_RESPONSE_NO_ERROR == write_block(buff, sector, count))
 {
  return RES_OK; /* Return result */
 }
 else
 {
  return RES_ERROR; /* Return result */
 }
}

DRESULT
FATdisk::disk_ioctl (BYTE drv, BYTE cmd, void* buff)
{
 DEBUG_LOG(0,"disk_ioctl drive=%u, cmd=%u\r\n", drv, cmd);
 if (drv)
  return RES_PARERR; /* Check parameter */
 if (SDCstat & STA_NOINIT)
  return RES_NOTRDY; /* Check if drive is ready */

 DWORD sector = 0;
 DWORD bs = 0;

 switch (cmd)
 {
  case CTRL_SYNC: /* Wait for end of internal write process of the drive */
   return RES_OK;

  case GET_SECTOR_COUNT: /* Get drive capacity in unit of sector (DWORD) */
   sector = get_card_capacity() / get_card_block_size();
   *(DWORD*) buff = sector;
   DEBUG_LOG(0,"sector = %u\r\n", *(DWORD*) buff);
   return RES_OK;

  case GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
   bs = get_card_block_size();
   *(DWORD*) buff = bs;
   DEBUG_LOG(0,"bs = %u\r\n", *(DWORD*) buff);
   return RES_OK;

  case CTRL_TRIM: /* Erase a block of sectors (used when _USE_ERASE == 1) */
   return RES_OK;
  default:
   return RES_PARERR;
 }
}

const char *
FATdisk::result_to_str (DRESULT r)
{
 switch(r)
 {
  case RES_OK:
   return "OK";
  case RES_ERROR:
   return "ERROR";
  case RES_WRPRT:
   return "WRPRT";
  case RES_NOTRDY:
   return "NOTRDY";
  case RES_PARERR:
   return "PARERR";
  default:
   return "Unknown error";
 }
}

const char *
FATdisk::result_to_str (FRESULT r)
{
 switch (r)
 {
  case FR_OK:
   return "OK";
  case FR_DISK_ERR:
   return "DISK ERR";
  case FR_INT_ERR:
   return "INT ERR";
  case FR_NOT_READY:
   return "NOT READY";
  case FR_NO_FILE:
   return "NO FILE";
  case FR_NO_PATH:
   return "NO PATH";
  case FR_INVALID_NAME:
   return "INVALID NAME";
  case FR_DENIED:
   return "DENIED";
  case FR_EXIST:
   return "EXIST";
  case FR_INVALID_OBJECT:
   return "INVALID OBJECT";
  case FR_WRITE_PROTECTED:
   return "WRITE PROTECTED";
  case FR_INVALID_DRIVE:
   return "INVALID DRIVE";
  case FR_NOT_ENABLED:
   return "NOT ENABLED";
  case FR_NO_FILESYSTEM:
   return "NO FILESYSTEM";
  case FR_MKFS_ABORTED:
   return "MKFS ABORTED";
  case FR_TIMEOUT:
   return "TIMEOUT";
  case FR_LOCKED:
   return "LOCKED";
  case FR_NOT_ENOUGH_CORE:
   return "NOT ENOUGH CORE";
  case FR_TOO_MANY_OPEN_FILES:
   return "TOO MANY OPEN FILES";
  case FR_INVALID_PARAMETER:
   return "INVALID PARAMETER";
  default:
   return "Unknown result";
 }
}
