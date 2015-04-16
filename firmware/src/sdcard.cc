#include <global.hpp>

DSTATUS disk_status (
  BYTE pdrv     /* [IN] Physical drive number */
)
{
	return 0;
}

DSTATUS disk_initialize (
  BYTE pdrv           /* [IN] Physical drive number */
)
{
	return 0;
}

DRESULT disk_read (
  BYTE pdrv,     /* [IN] Physical drive number */
  BYTE* buff,    /* [OUT] Pointer to the read data buffer */
  DWORD sector,  /* [IN] Start sector number */
  UINT count     /* [IN] Number of sectros to read */
)
{
	return RES_OK;
}


DRESULT disk_write (
  BYTE drv,         /* [IN] Physical drive number */
  const BYTE* buff, /* [IN] Pointer to the data to be written */
  DWORD sector,     /* [IN] Sector number to write from */
  UINT count        /* [IN] Number of sectors to write */
)
{
	return RES_OK;
}

DRESULT disk_ioctl (
  BYTE pdrv,     /* [IN] Drive number */
  BYTE cmd,      /* [IN] Control command code */
  void* buff     /* [I/O] Parameter and data buffer */
)
{
	return RES_OK;
}

DWORD get_fattime (void)
{
	return 0;
}