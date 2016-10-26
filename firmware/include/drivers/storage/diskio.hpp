#pragma once
#include <log.hpp>
#include <drivers/storage/fatfs/ff.h>
#include <drivers/storage/fatfs/diskio.h>
#include <drivers/console.hpp>
#include <drivers/storage/diskio.hpp>
#include <drivers/storage/sdc.hpp>

namespace DISK
{
class DiskIO : public SDC::Sdc
{
public:
	DiskIO (uint8_t channel);

public:
	static DSTATUS disk_initialize (BYTE pdrv);
	static DSTATUS disk_status (BYTE pdrv);
	static DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
	static DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
	static DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
	static DWORD get_fattime (void);

	static DiskIO *self;

protected:
	DSTATUS SDCstat;

};
}
