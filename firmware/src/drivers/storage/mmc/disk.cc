#include <drivers/storage/fat32/ff.h>
#include <drivers/mmc.hpp>
#include <drivers/console.hpp>
#include <drivers/console.hpp>

MMC::Mmc disk (1);

DSTATUS
disk_initialize (BYTE pdrv)
{
	return disk.disk_initialize ();
}

DSTATUS
disk_status (BYTE pdrv)
{
	return disk.disk_status ();
}

DRESULT
disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count)
{
	return disk.disk_read (pdrv, buff, sector, count);
}

DRESULT
disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count)
{
	return disk.disk_write (pdrv, buff, sector, count);
}

DRESULT
disk_ioctl (BYTE pdrv, BYTE cmd, void* buff)
{
	return disk.disk_ioctl (pdrv, cmd, buff);
}

const char *get_fresult(FRESULT r)
{
	switch(r)
	{
		case FR_OK:
			return "FR_OK";
		case FR_DISK_ERR:		
			return "FR_DISK_ERR";
		case FR_INT_ERR:			
			return "FR_INT_ERR";
		case FR_NOT_READY:		
			return "FR_NOT_READY";
		case FR_NO_FILE:			
			return "FR_NO_FILE";
		case FR_NO_PATH:
			return "FR_NO_PATH";			
		case FR_INVALID_NAME:	
			return "FR_INVALID_NAME";
		case FR_DENIED:	
			return "FR_DENIED";		
		case FR_EXIST:	
			return "FR_EXIST";		
		case FR_INVALID_OBJECT:	
			return "FR_INVALID_OBJECT";
		case FR_WRITE_PROTECTED:	
			return "FR_WRITE_PROTECTED";
		case FR_INVALID_DRIVE:	
			return "FR_INVALID_DRIVE";
		case FR_NOT_ENABLED:		
			return "FR_NOT_ENABLED";
		case FR_NO_FILESYSTEM:	
			return "FR_NO_FILESYSTEM";
		case FR_MKFS_ABORTED:		
			return "FR_MKFS_ABORTED";
		case FR_TIMEOUT:			
			return "FR_TIMEOUT";
		case FR_LOCKED:	
			return "FR_LOCKED";		
		case FR_NOT_ENOUGH_CORE:	
			return "FR_NOT_ENOUGH_CORE";
		case FR_TOO_MANY_OPEN_FILES:
			return "FR_TOO_MANY_OPEN_FILES";
		case FR_INVALID_PARAMETER:	
			return "FR_INVALID_PARAMETER";
		default:
			return "Unknown result";
	}
}

void
disk_test (void)
{
	Uart out(1, CONSOLE_SPEED);
	Console con(&out);
	con.print ("FatFs test started\r\n");
	FATFS fs;
	char path[4];
	FRESULT r = f_mount (&fs, path, 1);
	con.print("get_fresult(r)");
}
