#pragma once

#include <drivers/storage/sdc.hpp>
#include <drivers/storage/diskio.hpp>
#include <drivers/storage/fatdisk.hpp>
#include <drivers/storage/ff.hpp>
#include <drivers/storage/integer.hpp>
#include <sys/_stdint.h>

class FATdisk : public Sdc
{
public:
 FATdisk (uint8_t channel);

public:
 FRESULT open (FIL* fp, const TCHAR* path, BYTE mode);				/* Open or create a file */
 FRESULT close (FIL* fp);											/* Close an open file object */
 FRESULT f_read (FIL* fp, void* buff, UINT btr, UINT* br);			/* Read data from a file */
 FRESULT f_write (FIL* fp, const void* buff, UINT btw, UINT* bw);	/* Write data to a file */
 FRESULT forward (FIL* fp, UINT(*func)(const BYTE*,UINT), UINT btf, UINT* bf);	/* Forward data to the stream */
 FRESULT lseek (FIL* fp, DWORD ofs);								/* Move file pointer of a file object */
 FRESULT truncate (FIL* fp);										/* Truncate file */
 FRESULT sync (FIL* fp);											/* Flush cached data of a writing file */
 FRESULT f_opendir (DIR* dp, const TCHAR* path);						/* Open a directory */
 FRESULT closedir (DIR* dp);										/* Close an open directory */
 FRESULT f_readdir (DIR* dp, FILINFO* fno);							/* Read a directory item */
 FRESULT findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);	/* Find first file */
 FRESULT findnext (DIR* dp, FILINFO* fno);							/* Find next file */
 FRESULT mkdir (const TCHAR* path);								/* Create a sub directory */
 FRESULT unlink (const TCHAR* path);								/* Delete an existing file or directory */
 FRESULT rename (const TCHAR* path_old, const TCHAR* path_new);	/* Rename/Move a file or directory */
 FRESULT stat (const TCHAR* path, FILINFO* fno);					/* Get file status */
 FRESULT chmod (const TCHAR* path, BYTE attr, BYTE mask);			/* Change attribute of the file/dir */
 FRESULT utime (const TCHAR* path, const FILINFO* fno);			/* Change times-tamp of the file/dir */
 FRESULT chdir (const TCHAR* path);								/* Change current directory */
 FRESULT chdrive (const TCHAR* path);								/* Change current drive */
 FRESULT getcwd (TCHAR* buff, UINT len);							/* Get current directory */
 FRESULT getfree (const TCHAR* path, DWORD* nclst, FATFS** fatfs);	/* Get number of free clusters on the drive */
 FRESULT getlabel (const TCHAR* path, TCHAR* label, DWORD* vsn);	/* Get volume label */
 FRESULT setlabel (const TCHAR* label);							/* Set volume label */
 FRESULT mount (FATFS* fs, const TCHAR* path, BYTE opt);			/* Mount/Unmount a logical drive */
 FRESULT mkfs (const TCHAR* path, BYTE sfd, UINT au);				/* Create a file system on the volume */
 FRESULT fdisk (BYTE pdrv, const DWORD szt[], void* work);			/* Divide a physical drive into some partitions */
 int putc (TCHAR c, FIL* fp);										/* Put a character to the file */
 int puts (const TCHAR* str, FIL* cp);								/* Put a string to the file */
 int printf (FIL* fp, const TCHAR* str, ...);						/* Put a formatted string to the file */
 TCHAR* gets (TCHAR* buff, int len, FIL* fp);						/* Get a string from the file */

protected:
 //Basic low-level IO
 virtual DSTATUS disk_initialize (BYTE pdrv);
 virtual DSTATUS disk_status (BYTE pdrv);
 virtual DRESULT disk_read (BYTE pdrv, BYTE* buff, DWORD sector, UINT count);
 virtual DRESULT disk_write (BYTE pdrv, const BYTE* buff, DWORD sector, UINT count);
 virtual DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void* buff);
 virtual DWORD get_fattime (void);

protected:
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

public:
 const char *result_to_str (FRESULT r);
 const char *result_to_str (DRESULT r);

protected:
 DSTATUS SDCstat;

};

