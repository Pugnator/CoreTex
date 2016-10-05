#pragma once
#include <drivers/storage/mmc/fat32/diskio.h>
#include <hal/stm32f10x.hpp>
#include <global.hpp>
#include <common.hpp>
#include <hal/spi.hpp>

namespace HAL
{
using namespace SPI;
namespace MMC
{


/* MMC/SD command */
#define CMD0    (0)                     /* GO_IDLE_STATE */
#define CMD1    (1)                     /* SEND_OP_COND (MMC) */
#define ACMD41  (0x80+41)       /* SEND_OP_COND (SDC) */
#define CMD8    (8)                     /* SEND_IF_COND */
#define CMD9    (9)                     /* SEND_CSD */
#define CMD10   (10)            /* SEND_CID */
#define CMD12   (12)            /* STOP_TRANSMISSION */
#define ACMD13  (0x80+13)       /* SD_STATUS (SDC) */
#define CMD16   (16)            /* SET_BLOCKLEN */
#define CMD17   (17)            /* READ_SINGLE_BLOCK */
#define CMD18   (18)            /* READ_MULTIPLE_BLOCK */
#define CMD23   (23)            /* SET_BLOCK_COUNT (MMC) */
#define ACMD23  (0x80+23)       /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24   (24)            /* WRITE_BLOCK */
#define CMD25   (25)            /* WRITE_MULTIPLE_BLOCK */
#define CMD32   (32)            /* ERASE_ER_BLK_START */
#define CMD33   (33)            /* ERASE_ER_BLK_END */
#define CMD38   (38)            /* ERASE */
#define CMD55   (55)            /* APP_CMD */
#define CMD58   (58)            /* READ_OCR */

/* Disk Status Bits (DSTATUS) */
#define STA_NOINIT              0x01    /* Drive not initialized */
#define STA_NODISK              0x02    /* No medium in the drive */
#define STA_PROTECT             0x04    /* Write protected */


/* Command code for disk_ioctrl fucntion */

/* Generic command (Used by FatFs) */
#define CTRL_SYNC                       0       /* Complete pending write process (needed at _FS_READONLY == 0) */
#define GET_SECTOR_COUNT        1       /* Get media size (needed at _USE_MKFS == 1) */
#define GET_SECTOR_SIZE         2       /* Get sector size (needed at _MAX_SS != _MIN_SS) */
#define GET_BLOCK_SIZE          3       /* Get erase block size (needed at _USE_MKFS == 1) */
#define CTRL_TRIM                       4       /* Inform device that the data on the block of sectors is no longer used (needed at _USE_TRIM == 1) */

/* MMC card type flags (MMC_GET_TYPE) */
#define CT_MMC          0x01            /* MMC ver 3 */
#define CT_SD1          0x02            /* SD ver 1 */
#define CT_SD2          0x04            /* SD ver 2 */
#define CT_SDC          (CT_SD1|CT_SD2) /* SD */
#define CT_BLOCK        0x08            /* Block addressing */


class Mmc : public Spi
  {
public:
  Mmc(char ch) : Spi(ch)
  {
    channel = ch;
    MMCstat = STA_NOINIT;
    CardType = 0;
  }
  ~Mmc(void);
  DSTATUS disk_initialize (BYTE drv = 0);
  DSTATUS disk_status (BYTE drv = 0);
  DRESULT disk_read (BYTE drv,BYTE* buff,DWORD sector,UINT count);
  DRESULT disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count);
  DWORD get_fattime (void);
  DRESULT disk_ioctl (BYTE drv,BYTE cmd,void* buff);
protected:
  bool rcvr_datablock (BYTE *buff, UINT btr);
  bool xmit_datablock (const BYTE *buff, BYTE token);
  void mmcmultiread (BYTE *buff, UINT btr);
  void mmcmultiwrite (const BYTE *buff, UINT btx);
  uint16_t mmccmd(uint8_t cmd, word arg = 0);
  uint16_t mmcread(uint16_t data);
  void deselect();
  bool select();
  bool wait4ready(word how_long);
  int channel;
  DSTATUS MMCstat;    /* Physical drive status */
  BYTE CardType;
  };
}
}
