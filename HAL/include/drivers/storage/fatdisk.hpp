#pragma once

#include <drivers/storage/sdc.hpp>
#include <drivers/storage/diskio.hpp>
#include <drivers/storage/fatdisk.hpp>
#include <drivers/storage/ff.h>
#include <drivers/storage/integer.hpp>
#include <sys/_stdint.h>

typedef struct
{
  FIL *fp;
  int idx, nchr;
  BYTE buf[64];
} putbuff;

class FATdisk : public Sdc
{
public:
  FATdisk(uint8_t channel);

public:
  FRESULT f_open(FIL *fp, const TCHAR *path, BYTE mode);                            /* Open or create a file */
  FRESULT f_close(FIL *fp);                                                         /* Close an open file object */
  FRESULT f_read(FIL *fp, void *buff, UINT btr, UINT *br);                          /* Read data from a file */
  FRESULT f_write(FIL *fp, const void *buff, UINT btw, UINT *bw);                   /* Write data to a file */
  FRESULT f_forward(FIL *fp, UINT (*func)(const BYTE *, UINT), UINT btf, UINT *bf); /* Forward data to the stream */
  FRESULT f_lseek(FIL *fp, FSIZE_t ofs);                                            /* Move file pointer of a file object */
  FRESULT f_truncate(FIL *fp);                                                      /* Truncate file */
  FRESULT f_flush(FIL *fp);                                                         /* Flush cached data of a writing file */
  FRESULT f_sync(FIL *fp);
  FRESULT f_opendir(DIR *dp, const TCHAR *path);                                       /* Open a directory */
  FRESULT f_closedir(DIR *dp);                                                         /* Close an open directory */
  FRESULT f_readdir(DIR *dp, FILINFO *fno);                                            /* Read a directory item */
  FRESULT f_findfirst(DIR *dp, FILINFO *fno, const TCHAR *path, const TCHAR *pattern); /* Find first file */
  FRESULT f_findnext(DIR *dp, FILINFO *fno);                                           /* Find next file */
  FRESULT f_mkdir(const TCHAR *path);                                                  /* Create a sub directory */
  FRESULT f_unlink(const TCHAR *path);                                                 /* Delete an existing file or directory */
  FRESULT f_rename(const TCHAR *path_old, const TCHAR *path_new);                      /* Rename/Move a file or directory */
  FRESULT f_stat(const TCHAR *path, FILINFO *fno);                                     /* Get file status */
  FRESULT f_chmod(const TCHAR *path, BYTE attr, BYTE mask);                            /* Change attribute of the file/dir */
  FRESULT f_utime(const TCHAR *path, const FILINFO *fno);                              /* Change times-tamp of the file/dir */
  FRESULT f_chdir(const TCHAR *path);                                                  /* Change current directory */
  FRESULT f_chdrive(const TCHAR *path);                                                /* Change current drive */
  FRESULT f_getcwd(TCHAR *buff, UINT len);                                             /* Get current directory */
  FRESULT f_getfree(const TCHAR *path, DWORD *nclst, FATFS **fatfs);                   /* Get number of free clusters on the drive */
  FRESULT f_getlabel(const TCHAR *path, TCHAR *label, DWORD *vsn);                     /* Get volume label */
  FRESULT f_setlabel(const TCHAR *label);                                              /* Set volume label */
  FRESULT f_mount(FATFS *fs, const TCHAR *path, BYTE opt);                             /* Mount/Unmount a logical drive */
  FRESULT f_mkfs(const TCHAR *path, const MKFS_PARM *opt, void *work, UINT len);
  FRESULT f_fdisk(BYTE pdrv, const DWORD szt[], void *work); /* Divide a physical drive into some partitions */
  FRESULT f_dir_remove(DIR *dp);
  int putc(TCHAR c, FIL *fp);                 /* Put a character to the file */
  int puts(const TCHAR *str, FIL *cp);        /* Put a string to the file */
  int printf(FIL *fp, const TCHAR *str, ...); /* Put a formatted string to the file */
  TCHAR *gets(TCHAR *buff, int len, FIL *fp); /* Get a string from the file */

protected:
  //Basic low-level IO
  virtual DSTATUS disk_initialize(BYTE pdrv);
  virtual DSTATUS disk_status(BYTE pdrv);
  virtual DRESULT disk_read(BYTE pdrv, BYTE *buff, DWORD sector, UINT count);
  virtual DRESULT disk_write(BYTE pdrv, const BYTE *buff, DWORD sector, UINT count);
  virtual DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff);
  virtual DWORD get_fattime(void);

protected:
  void putc_bfd(putbuff *pb, TCHAR c);
  FRESULT sync_window(FATFS *fs);
  FRESULT move_window(FATFS *fs, DWORD sector);
  FRESULT sync_fs(FATFS *fs);
  DWORD clust2sect(FATFS *fs, DWORD clst);
  DWORD get_fat(FFOBJID *obj, DWORD clst);
  FRESULT put_fat(FATFS *fs, DWORD clst, DWORD val);
  FRESULT remove_chain(FATFS *fs, DWORD clst);
  DWORD create_chain(FFOBJID *fs, DWORD clst);
  FRESULT create_partition(BYTE drv, const LBA_t plst[], UINT sys, BYTE *buf);
  FRESULT dir_sdi(DIR *dp, DWORD ofs);
  FRESULT dir_next(DIR *dp, int stretch);
  FRESULT dir_alloc(DIR *dp, UINT nent);
  DWORD ld_clust(FATFS *fs, const BYTE *dir);
  void st_clust(FATFS *fs, BYTE *dir, DWORD cl);
  FRESULT dir_find(DIR *dp);
  FRESULT dir_read(DIR *dp, int vol);
  FRESULT dir_register(DIR *dp);
  FRESULT dir_clear(FATFS *fs, DWORD clst);
  FRESULT dir_remove(DIR *dp);
  DWORD find_bitmap(FATFS *fs, DWORD clst, DWORD ncl);
  FRESULT change_bitmap(FATFS *fs, DWORD clst, DWORD ncl, int bv);
  FRESULT fill_first_frag(FFOBJID *obj);
  FRESULT fill_last_frag(FFOBJID *obj, DWORD lcl, DWORD term);
  void create_xdir(BYTE *dirb, const WCHAR *lfn);
  FRESULT load_xdir(DIR *dp);
  FRESULT store_xdir(DIR *dp);
  FRESULT load_obj_xdir(DIR *dp, const FFOBJID *obj);
  FRESULT remove_chain(FFOBJID *obj, DWORD clst, DWORD pclst);
  FRESULT mount_volume(const TCHAR **path, FATFS **rfs, BYTE mode);
  void get_fileinfo(DIR *dp, FILINFO *fno);
  FRESULT create_name(DIR *dp, const TCHAR **path);
  FRESULT follow_path(DIR *dp, const TCHAR *path);
  int get_ldnumber(const TCHAR **path);
  UINT check_fs(FATFS *fs, LBA_t sect);
  UINT find_volume(FATFS *fs, UINT part);
  FRESULT validate(FFOBJID *obj, FATFS **rfs);

public:
  const char *result_to_str(FRESULT r);
  const char *result_to_str(DRESULT r);

protected:
  DSTATUS SDCstat;
};
