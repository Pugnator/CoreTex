#include <drivers/storage/fat32/ff.h>
#include <drivers/mmc.hpp>
#include <drivers/console.hpp>

HAL::MMC::Mmc disk(1);

DSTATUS disk_initialize (BYTE pdrv)
{
	return disk.disk_initialize();
}

DSTATUS disk_status (BYTE pdrv)
{
	return disk.disk_status();
}

DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	return disk.disk_read(pdrv, buff, sector, count);
}

DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
	return disk.disk_write(pdrv, buff, sector, count);
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
	return disk.disk_ioctl(pdrv, cmd, buff);
}


void disk_test(CONSOLE::Console *con)
{
	con->print("FatFs test started\r\n");
	DSTATUS stat = STA_NOINIT;
	stat = disk.disk_initialize(0);
	FATFS fs;
	char path[4];
	f_mount(&fs, path, 0);
}
