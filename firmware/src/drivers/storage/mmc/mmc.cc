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
#include <drivers/storage/fat32/diskio.h>
#include <global.hpp>
#include <hal/stm32f10x.hpp>
#include <hal/io_macro.hpp>
#include <drivers/mmc.hpp>

namespace MMC
{
bool Mmc::select(void)
  {
    PIN_LOW(SPI1NSS_PIN);
    /* Dummy clock (force DO enabled) */
    read(0xff);
    //if (wait4ready(500)) return true;      /* Wait for card ready */
    //deselect();
    return true;
  }

void Mmc::deselect(void)
  {
    PIN_HI(SPI1NSS_PIN);
    /* Dummy clock (force DO hi-z for multiple slave SPI) */
    read(0xff);
  }

//FIXME: should work, but don't at the moment
bool Mmc::wait4ready(word how_long)
  {
    uint16_t d;
    WAIT_FOR(how_long);
    do
      {
        d = read(0xFF);
      }
    while (d != 0xFF && STILL_WAIT); /* Wait for card goes ready or timeout */

    return (d == 0xFF) ? true : false;
  }

DSTATUS Mmc::disk_status(BYTE drv)
  {
    return drv ? STA_NOINIT : MMCstat;
  }

DRESULT Mmc::disk_read(BYTE drv, BYTE* buff, DWORD sector, UINT count)
  {
    if (drv || !count) return RES_PARERR; /* Check parameter */
    if (MMCstat & STA_NOINIT) return RES_NOTRDY; /* Check if drive is ready */

    if (!(CardType & CT_BLOCK)) sector *= 512; /* LBA ot BA conversion (byte addressing cards) */

    if (count == 1)
      { /* Single sector read */
        if ((mmccmd(CMD17, sector) == 0) /* READ_SINGLE_BLOCK */
        && rcvr_datablock(buff, 512)) count = 0;
      }
    else
      { /* Multiple sector read */
        if (mmccmd(CMD18, sector) == 0)
          { /* READ_MULTIPLE_BLOCK */
            do
              {
                if (!rcvr_datablock(buff, 512)) break;
                buff += 512;
              }
            while (--count);
            mmccmd(CMD12); /* STOP_TRANSMISSION */
          }
      }
    deselect();

    return count ? RES_ERROR : RES_OK; /* Return result */
  }

bool Mmc::rcvr_datablock(BYTE *buff, UINT btr)
  {
    BYTE token;

    WAIT_FOR(200);
    do
      { /* Wait for DataStart token in timeout of 200ms */
        token = read(0xFF);
        /* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
      }
    while ((token == 0xFF) && STILL_WAIT);
    if (token != 0xFE) return false; /* Function fails if invalid DataStart token or timeout */

    mmcmultiread(buff, btr); /* Store trailing data to the buffer */
    read(0xFF);
    read(0xFF);
    return true; /* Function succeeded */
  }

bool Mmc::xmit_datablock(const BYTE *buff, BYTE token)
  {
    BYTE resp;

    if (!wait4ready(500))
      {
        return false; /* Wait for card ready */
      }

    read(token); /* Send token */
    if (token != 0xFD)
      { /* Send data if token is other than StopTran */
        mmcmultiwrite(buff, 512); /* Data */
        /* Dummy CRC */
        read(0xFF);
        read(0xFF);

        resp = read(0xFF); /* Receive data resp */
        if ((resp & 0x1F) != 0x05) /* Function fails if the data packet was not accepted */
        return false;
      }
    return true;
  }

void Mmc::mmcmultiread(BYTE *buff, UINT btr)
  {
    while (Reg->SR & SPI_SR_BSY)
      ;
    WORD d;
    /* Set SPI to 16-bit mode */
    go16bit();

    Reg->DR = 0xFFFF;
    btr -= 2;
    do
      { /* Receive the data block into buffer */
        while (Reg->SR & SPI_SR_BSY)
          ;
        d = Reg->DR;
        Reg->DR = 0xFFFF;
        buff[1] = d;
        buff[0] = d >> 8;
        buff += 2;
      }
    while (btr -= 2);

    while (Reg->SR & SPI_SR_BSY)
      ;
    d = Reg->DR;
    buff[1] = d;
    buff[0] = d >> 8;

    /* Set SPI to 8-bit mode */
    go8bit();
  }

void Mmc::mmcmultiwrite(const BYTE *buff, UINT btx)
  {
    WORD d;
    go16bit();

    d = buff[0] << 8 | buff[1];
    Reg->DR = d;
    buff += 2;
    btx -= 2;
    do
      { /* Receive the data block into buffer */
        d = buff[0] << 8 | buff[1];
        while (Reg->SR & SPI_SR_BSY)
          ;
        Reg->DR;
        Reg->DR = d;
        buff += 2;
      }
    while (btx -= 2);
    while (Reg->SR & SPI_SR_BSY)
      ;
    Reg->DR;

    go8bit();
  }

DRESULT Mmc::disk_write(BYTE drv, const BYTE* buff, DWORD sector, UINT count)
  {
    if (drv || !count) return RES_PARERR; /* Check parameter */
    if (MMCstat & STA_NOINIT) return RES_NOTRDY; /* Check drive status */
    if (MMCstat & STA_PROTECT) return RES_WRPRT; /* Check write protect */

    if (!(CardType & CT_BLOCK)) sector *= 512; /* LBA ==> BA conversion (byte addressing cards) */

    if (count == 1)
      { /* Single sector write */
        if (0 == mmccmd(CMD24, sector))
          {
            /* WRITE_BLOCK */
            if (xmit_datablock(buff, 0xFE))
              {
                count = 0;
              }
          }
      }
    else
      { /* Multiple sector write */
        if (CardType & CT_SDC) mmccmd(ACMD23, count); /* Predefine number of sectors */
        if (mmccmd(CMD25, sector) == 0)
          { /* WRITE_MULTIPLE_BLOCK */
            do
              {
                if (!xmit_datablock(buff, 0xFC)) break;
                buff += 512;
              }
            while (--count);
            if (!xmit_datablock(0, 0xFD)) /* STOP_TRAN token */
            count = 1;
          }
      }
    deselect();

    return count ? RES_ERROR : RES_OK; /* Return result */
  }

//FIXME: get current system time
DWORD Mmc::get_fattime(void)
  {
    return 0;
  }

DRESULT Mmc::disk_ioctl(BYTE drv, BYTE cmd, void* buff)
  {
    DRESULT res;
    BYTE n, csd[16];
    DWORD *dp, st, ed, csize;

    if (drv) return RES_PARERR; /* Check parameter */
    if (MMCstat & STA_NOINIT) return RES_NOTRDY; /* Check if drive is ready */

    res = RES_ERROR;

    switch (cmd)
      {
      case CTRL_SYNC: /* Wait for end of internal write process of the drive */
        if (select()) res = RES_OK;
        break;

      case GET_SECTOR_COUNT: /* Get drive capacity in unit of sector (DWORD) */
        if ((mmccmd(CMD9) == 0) && rcvr_datablock(csd, 16))
          {
            if ((csd[0] >> 6) == 1)
              { /* SDC ver 2.00 */
                csize = csd[9] + ((WORD) csd[8] << 8)
                    + ((DWORD) (csd[7] & 63) << 16) + 1;
                *(DWORD*) buff = csize << 10;
              }
            else
              { /* SDC ver 1.XX or MMC ver 3 */
                n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1)
                    + 2;
                csize = (csd[8] >> 6) + ((WORD) csd[7] << 2)
                    + ((WORD) (csd[6] & 3) << 10) + 1;
                *(DWORD*) buff = csize << (n - 9);
              }
            res = RES_OK;
          }
        break;

      case GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
        if (CardType & CT_SD2)
          { /* SDC ver 2.00 */
            if (mmccmd(ACMD13) == 0)
              { /* Read SD status */
                mmccmd(0xFF);
                if (rcvr_datablock(csd, 16))
                  { /* Read partial block */
                    for (n = 64 - 16; n; n--)
                    	read(0xFF); /* Purge trailing data */
                    *(DWORD*) buff = 16UL << (csd[10] >> 4);
                    res = RES_OK;
                  }
              }
          }
        else
          { /* SDC ver 1.XX or MMC */
            if ((mmccmd(CMD9) == 0) && rcvr_datablock(csd, 16))
              { /* Read CSD */
                if (CardType & CT_SD1)
                  { /* SDC ver 1.XX */
                    *(DWORD*) buff = (((csd[10] & 63) << 1)
                        + ((WORD) (csd[11] & 128) >> 7) + 1)
                        << ((csd[13] >> 6) - 1);
                  }
                else
                  { /* MMC */
                    *(DWORD*) buff = ((WORD) ((csd[10] & 124) >> 2) + 1)
                        * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
                  }
                res = RES_OK;
              }
          }
        break;

      case CTRL_TRIM: /* Erase a block of sectors (used when _USE_ERASE == 1) */
        if (!(CardType & CT_SDC)) break; /* Check if the card is SDC */
        if (disk_ioctl(drv, MMC_GET_CSD, csd)) break; /* Get CSD */
        if (!(csd[0] >> 6) && !(csd[10] & 0x40)) break; /* Check if sector erase can be applied to the card */
        dp = (DWORD*) buff;
        st = dp[0];
        ed = dp[1]; /* Load sector block */
        if (!(CardType & CT_BLOCK))
          {
            st *= 512;
            ed *= 512;
          }
        if (mmccmd(CMD32, st) == 0 && mmccmd(CMD33, ed) == 0
            && mmccmd(CMD38) == 0 && wait4ready(30000)) /* Erase sector block */
        res = RES_OK; /* FatFs does not check result of this command */
        break;

      default:
        res = RES_PARERR;
      }

    deselect();
    return res;
  }

uint16_t Mmc::mmccmd(uint8_t command, word arg)
  {
    uint8_t n, res;

    if (command & 0x80)
      { /* Send a CMD55 prior to ACMD<n> */
        command &= 0x7F;
        res = mmccmd(CMD55);
        if (res > 1) return res;
      }

    /* Select the card and wait for ready except to stop multiple block read */
    if (command != CMD12)
      {
        deselect();
        if (!select()) return 0xFF;
      }

    /* Send command packet */
    read(0x40 | command); /* Start + command index */
    read((BYTE) (arg >> 24)); /* Argument[31..24] */
    read((BYTE) (arg >> 16)); /* Argument[23..16] */
    read((BYTE) (arg >> 8)); /* Argument[15..8] */
    read((BYTE) arg); /* Argument[7..0] */
    n = 0x01; /* Dummy CRC + Stop */
    if (command == CMD0) n = 0x95; /* Valid CRC for CMD0(0) */
    if (command == CMD8) n = 0x87; /* Valid CRC for CMD8(0x1AA) */
    read(n);

    /* Receive command resp */
    if (command == CMD12) read(0xFF); /* Diacard following one byte when CMD12 */
    n = 10; /* Wait for response (10 bytes max) */
    do
      res = read(0xFF);
    while ((res & 0x80) && --n);

    return res;
  }

const char *Mmc::get_status()
{
	disk_initialize(0);
	switch (MMCstat)
	{
		case RES_OK:
			return "Successful\r\n";
		case RES_NOTRDY:
			return "Not Ready\r\n";
		case RES_ERROR:
			return "R/W Error\r\n";
		case RES_PARERR:
			return "Invalid Parameter\r\n";
		case RES_WRPRT:
			return "Write Protected\r\n";
		default:
			return "Unknown error\r\n";
	}
}

DSTATUS Mmc::disk_initialize(BYTE drv)
  {
    BYTE cmd, ty, ocr[4];
    if (drv) return STA_NOINIT; /* Supports only drive 0 */

    if (MMCstat & STA_NODISK) return MMCstat; /* Is card existing in the soket? */

    for (word n = 20; n; n--)
      read(0xFF); /* Send 80 dummy clocks */

    ty = 0;
    if (mmccmd(CMD0) == 1)
      { /* Put the card SPI/Idle state */
        WAIT_FOR(1000); /* Initialization timeout = 1 sec */
        if (mmccmd(CMD8, 0x1AA) == 1)
          { /* SDv2? */
            for (word n = 0; n < 4; n++)
              ocr[n] = read(0xFF); /* Get 32 bit return value of R7 resp */
            if (ocr[2] == 0x01 && ocr[3] == 0xAA)
              { /* Is the card supports vcc of 2.7-3.6V? */
                while (STILL_WAIT && mmccmd(ACMD41, 1UL << 30))
                  ; /* Wait for end of initialization with ACMD41(HCS) */
                if (STILL_WAIT && mmccmd(CMD58) == 0)
                  { /* Check CCS bit in the OCR */
                    for (word n = 0; n < 4; n++)
                      ocr[n] = mmccmd(0xFF);
                    ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; /* Card id SDv2 */
                  }
              }
          }
        else
          { /* Not SDv2 card */
            if (mmccmd(ACMD41) <= 1)
              { /* SDv1 or MMC? */
                ty = CT_SD1;
                cmd = ACMD41; /* SDv1 (ACMD41(0)) */
              }
            else
              {
                ty = CT_MMC;
                cmd = CMD1; /* MMCv3 (CMD1(0)) */
              }
            while (STILL_WAIT && mmccmd(cmd, 0))
              ; /* Wait for end of initialization */
            if (!STILL_WAIT || mmccmd(CMD16, 512) != 0) /* Set block length: 512 */
            ty = 0;
          }
      }
    CardType = ty; /* Card type */
    deselect();

    if (ty)
      {
        /* Set fast clock */
        high_speed();
        MMCstat &= ~STA_NOINIT; /* Clear STA_NOINIT flag */
      }
    else
      { /* Failed */
        MMCstat = STA_NOINIT;
      }
    return MMCstat;
  }
}

