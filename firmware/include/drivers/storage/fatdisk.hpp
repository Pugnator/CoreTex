#pragma once
#include <drivers/storage/ff.hpp>
#include <drivers/storage/diskio.hpp>
#include <drivers/storage/sdc.hpp>
#include <drivers/storage/fatdisk.hpp>

namespace DISK
{
class FATdisk : public SDC::Sdc
{
public:
	FATdisk (uint8_t channel);

public:
	FRESULT f_open (FIL* fp, const TCHAR* path, BYTE mode);				/* Open or create a file */
	FRESULT f_close (FIL* fp);											/* Close an open file object */
	FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from a file */
	FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to a file */
	FRESULT f_forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* Forward data to the stream */
	FRESULT f_lseek (FIL* fp, DWORD ofs);								/* Move file pointer of a file object */
	FRESULT f_truncate (FIL* fp);										/* Truncate file */
	FRESULT f_sync (FIL* fp);											/* Flush cached data of a writing file */
	FRESULT f_opendir (DIR* dp, const TCHAR* path);						/* Open a directory */
	FRESULT f_closedir (DIR* dp);										/* Close an open directory */
	FRESULT f_readdir (DIR* dp, FILINFO* fno);							/* Read a directory item */
	FRESULT f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);	/* Find first file */
	FRESULT f_findnext (DIR* dp, FILINFO* fno);							/* Find next file */
	FRESULT f_mkdir (const TCHAR* path);								/* Create a sub directory */
	FRESULT f_unlink (const TCHAR* path);								/* Delete an existing file or directory */
	FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory */
	FRESULT f_stat (const TCHAR* path, FILINFO* fno);					/* Get file status */
	FRESULT f_chmod (const TCHAR* path, BYTE attr, BYTE mask);			/* Change attribute of the file/dir */
	FRESULT f_utime (const TCHAR* path, const FILINFO* fno);			/* Change times-tamp of the file/dir */
	FRESULT f_chdir (const TCHAR* path);								/* Change current directory */
	FRESULT f_chdrive (const TCHAR* path);								/* Change current drive */
	FRESULT f_getcwd (TCHAR* buff, UINT len);							/* Get current directory */
	FRESULT f_getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* Get number of free clusters on the drive */
	FRESULT f_getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn);	/* Get volume label */
	FRESULT f_setlabel (const TCHAR* label);							/* Set volume label */
	FRESULT f_mount (FATFS* fs, const TCHAR* path, BYTE opt);			/* Mount/Unmount a logical drive */
	FRESULT f_mkfs (const TCHAR* path, BYTE sfd, UINT au);				/* Create a file system on the volume */
	FRESULT f_fdisk (BYTE pdrv, const DWORD szt[], void* work);			/* Divide a physical drive into some partitions */
	int f_putc (TCHAR c, FIL* fp);										/* Put a character to the file */
	int f_puts (const TCHAR* str, FIL* cp);								/* Put a string to the file */
	int f_printf (FIL* fp, const TCHAR* str, ...);						/* Put a formatted string to the file */
	TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);						/* Get a string from the file */

protected:
	//Basic low-level IO
	DSTATUS disk_initialize (BYTE pdrv);
	DSTATUS disk_status (BYTE pdrv);
	DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
	DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
	DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
	DWORD get_fattime (void);


	FRESULT	sync_window (FATFS* fs);
	FRESULT move_window(FATFS* fs,	DWORD sector);
	FRESULT	sync_fs (FATFS* fs);
	DWORD clust2sect(FATFS* fs, DWORD clst	);
	DWORD get_fat(	FATFS* fs, DWORD clst);
	FRESULT	put_fat (FATFS* fs,	DWORD clst,	DWORD val	);
	FRESULT	remove_chain (FATFS* fs,DWORD clst	);
	DWORD create_chain (FATFS* fs,	DWORD clst	);
	FRESULT dir_sdi(DIR* dp, UINT idx);
	FRESULT dir_next(DIR* dp, int stretch);
	FRESULT	dir_alloc (DIR* dp,	UINT nent	);
	DWORD ld_clust(FATFS* fs,BYTE* dir);
	void st_clust (BYTE* dir,DWORD cl);
	FRESULT dir_find(DIR* dp);
	FRESULT dir_read(DIR* dp, int vol);
	FRESULT	dir_register (DIR* dp);
	void get_fileinfo(DIR* dp,	FILINFO* fno);
	FRESULT create_name(DIR* dp,const TCHAR** path);
	FRESULT follow_path(DIR* dp,const TCHAR* path);
	int get_ldnumber(const TCHAR** path	);
	BYTE check_fs(FATFS* fs, DWORD sect );
	FRESULT find_volume(FATFS** rfs,const TCHAR** path,	BYTE wmode);
	FRESULT validate(void* obj );

	const char *DRESULT2str (DRESULT r);
	const char *FRESULT2str (FRESULT r);

protected:
	DSTATUS SDCstat;

};
}
