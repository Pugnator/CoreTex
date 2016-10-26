#include <core/spi.hpp>
#include <global.hpp>
#include <common.hpp>
#include <log.hpp>
#include <drivers/storage/fatfs/ff.h>
#include <drivers/storage/fatfs/diskio.h>
#include <drivers/console.hpp>
#include <drivers/storage/diskio.hpp>
#include <drivers/storage/sdc.hpp>

DSTATUS (*disk_initialize) (BYTE pdrv);
DSTATUS (*disk_status) (BYTE pdrv);
DRESULT (*disk_read) (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
DRESULT (*disk_write) (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
DRESULT (*disk_ioctl) (BYTE pdrv, BYTE cmd, void* buff);
DWORD (*get_fattime) (void);

using namespace SDC;

namespace DISK
{
class DiskIO *DiskIO::self = nullptr;

DiskIO::DiskIO (uint8_t channel)
: SDC::Sdc(channel)
{
	self = this;
	::disk_initialize = &disk_initialize;
	::disk_status = &disk_status;
	::disk_read = &disk_read;
	::disk_write = &disk_write;
	::disk_ioctl = &disk_ioctl;
	::get_fattime = &get_fattime;
	SDCstat = 0;
}

DWORD DiskIO::get_fattime (void)
{
	return 0;
}

DSTATUS
DiskIO::disk_initialize (BYTE drv)
{
	DBGPRINT("disk_initialize: Drive=%u\r\n", drv);
	self->lowspeed();
	self->go8bit();
	if (drv)
	{
		DBGPRINT("disk_initialize: STA_NOINIT\r\n");
		return STA_NOINIT; /* Supports only drive 0 */
	}

	if (self->SDCstat & STA_NODISK)
	{
		DBGPRINT("disk_initialize: MMCstat=%u\r\n", self->SDCstat);
		return self->SDCstat; /* Is card existing in the soket? */
	}

	if( SD_RESPONSE_FAILURE == self->initialize())
	{
		LOGPRINT("SD init failed\r\n");
		self->SDCstat = STA_NOINIT;
	}
	else
	{
		LOGPRINT("SD init OK\r\n");
		self->highspeed();
		self->SDCstat &= ~STA_NOINIT;
	}
	return self->SDCstat;
}

DSTATUS
DiskIO::disk_status (BYTE drv)
{
	return drv ? STA_NOINIT : self->SDCstat;
}

DRESULT
DiskIO::disk_read (BYTE drv, BYTE* buff, DWORD sector, UINT count)
{
	DBGPRINT("disk_read: Drive=%u, sector=%u, count=%u\r\n", drv, sector, count);
	if(SD_RESPONSE_NO_ERROR == self->read_block(buff, sector * _MIN_SS, count * _MIN_SS))
	{
		return RES_OK; /* Return result */
	}
	else
	{
		return RES_ERROR; /* Return result */
	}
}

DRESULT
DiskIO::disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
	DBGPRINT("disk_write: Drive=%u, sector=%u, count=%u\r\n", drv, sector, count);
	if(SD_RESPONSE_NO_ERROR == self->write_block(buff, sector * _MIN_SS, count * _MIN_SS))
	{
		return RES_OK; /* Return result */
	}
	else
	{
		return RES_ERROR; /* Return result */
	}
}

DRESULT
DiskIO::disk_ioctl (BYTE drv, BYTE cmd, void* buff)
{
	LOGPRINT("disk_ioctl drive=%u, cmd=%u\r\n", drv, cmd);
	if (drv)
		return RES_PARERR; /* Check parameter */
	if (self->SDCstat & STA_NOINIT)
		return RES_NOTRDY; /* Check if drive is ready */

	DWORD sector = 0;
	DWORD bs = 0;

	switch (cmd)
	{
	case CTRL_SYNC: /* Wait for end of internal write process of the drive */
		return RES_OK;

	case GET_SECTOR_COUNT: /* Get drive capacity in unit of sector (DWORD) */
		sector = self->get_card_capacity() / self->get_card_block_size();
		*(DWORD*) buff = sector;
		LOGPRINT("sector = %u\r\n", *(DWORD*) buff);
		return RES_OK;

	case GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
		bs = self->get_card_block_size();
		*(DWORD*) buff = bs;
		LOGPRINT("bs = %u\r\n", *(DWORD*) buff);
		return RES_OK;

	case CTRL_TRIM: /* Erase a block of sectors (used when _USE_ERASE == 1) */
		return RES_OK;
	default:
		return RES_PARERR;
	}
}
}
