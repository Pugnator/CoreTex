#include <core/spi.hpp>
#include <global.hpp>
#include <drivers/storage/fat32/ff.h>
#include <drivers/storage/fat32/diskio.h>
#include <drivers/storage/disk.hpp>
#include <drivers/console.hpp>

static SPI::Spi disk (1);
static DSTATUS MMCstat; /* Physical drive status */
static BYTE CardType;

DWORD get_fattime (void)
{
	return 0;
}

bool
wait4ready (word how_long)
{
	uint16_t d;
	WAIT_FOR (how_long);
	do
		{
			d = disk.read (0xFF);
		}
	while (d != 0xFF && STILL_WAIT); /* Wait for card goes ready or timeout */

	return (d == 0xFF) ? true : false;
}

bool
select (void)
{
	PIN_LOW (SPI1NSS_PIN);
	/* Dummy clock (force DO enabled) */
	disk.read (0xff);
	//if (wait4ready(500)) return true;      /* Wait for card ready */
	//deselect();
	return true;
}

void
deselect (void)
{
	PIN_HI (SPI1NSS_PIN);
	/* Dummy clock (force DO hi-z for multiple slave SPI) */
	disk.read (0xff);
}

uint16_t
mmccmd (uint8_t command, word arg)
{
	while(0xFF != disk.read(0xFF));
	uint8_t n, res;

	if (command & 0x80)
		{ /* Send a CMD55 prior to ACMD<n> */
			command &= 0x7F;
			res = mmccmd (CMD55, 0);
			if (res > 1)
				return res;
		}

	/* Select the card and wait for ready except to stop multiple block read */
	if (command != CMD12)
		{
			deselect ();
			if (!select ())
				return 0xFF;
		}

	/* Send command packet */
	disk.read (0x40 | command); /* Start + command index */
	disk.read ((BYTE) (arg >> 24)); /* Argument[31..24] */
	disk.read ((BYTE) (arg >> 16)); /* Argument[23..16] */
	disk.read ((BYTE) (arg >> 8)); /* Argument[15..8] */
	disk.read ((BYTE) arg); /* Argument[7..0] */
	n = 0x01; /* Dummy CRC + Stop */
	if (command == CMD0)
		n = 0x95; /* Valid CRC for CMD0(0) */
	if (command == CMD8)
		n = 0x87; /* Valid CRC for CMD8(0x1AA) */
	disk.read (n);

	/* Receive command resp */
	if (command == CMD12)
		disk.read (0xFF); /* Diacard following one byte when CMD12 */
	n = 10; /* Wait for response (10 bytes max) */
	do
		res = disk.read (0xFF);
	while ((res & 0x80) && --n);

	return res;
}

DSTATUS
disk_initialize (BYTE drv)
{
	BYTE cmd, ty, ocr[4];
	if (drv)
		return STA_NOINIT; /* Supports only drive 0 */

	if (MMCstat & STA_NODISK)
		return MMCstat; /* Is card existing in the soket? */

	for (word n = 20; n; n--)
		disk.read (0xFF); /* Send 80 dummy clocks */

	ty = 0;
	if (mmccmd (CMD0, 0) == 1)
		{ /* Put the card SPI/Idle state */
			WAIT_FOR (1000); /* Initialization timeout = 1 sec */
			if (mmccmd (CMD8, 0x1AA) == 1)
				{ /* SDv2? */
					for (word n = 0; n < 4; n++)
						ocr[n] = disk.read (0xFF); /* Get 32 bit return value of R7 resp */
					if (ocr[2] == 0x01 && ocr[3] == 0xAA)
						{ /* Is the card supports vcc of 2.7-3.6V? */
							while (STILL_WAIT && mmccmd (ACMD41, 1UL << 30))
								; /* Wait for end of initialization with ACMD41(HCS) */
							if (STILL_WAIT && mmccmd (CMD58, 0) == 0)
								{ /* Check CCS bit in the OCR */
									for (word n = 0; n < 4; n++)
										ocr[n] = mmccmd (0xFF, 0);
									ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; /* Card id SDv2 */
								}
						}
				}
			else
				{ /* Not SDv2 card */
					if (mmccmd (ACMD41, 0) <= 1)
						{ /* SDv1 or MMC? */
							ty = CT_SD1;
							cmd = ACMD41; /* SDv1 (ACMD41(0)) */
						}
					else
						{
							ty = CT_MMC;
							cmd = CMD1; /* MMCv3 (CMD1(0)) */
						}
					while (STILL_WAIT && mmccmd (cmd, 0))
						; /* Wait for end of initialization */
					if (!STILL_WAIT || mmccmd (CMD16, 512) != 0) /* Set block length: 512 */
						ty = 0;
				}
		}
	CardType = ty; /* Card type */
	deselect ();

	if (ty)
		{
			/* Set fast clock */
			disk.high_speed ();
			MMCstat &= ~STA_NOINIT; /* Clear STA_NOINIT flag */
		}
	else
		{ /* Failed */
			MMCstat = STA_NOINIT;
		}
	return MMCstat;

}

bool
rcvr_datablock (BYTE *buff, UINT btr)
{
	BYTE token;

	WAIT_FOR (200);
	do
		{ /* Wait for DataStart token in timeout of 200ms */
			token = disk.read (0xFF);
			/* This loop will take a time. Insert rot_rdq() here for multitask envilonment. */
		}
	while ((token == 0xFF) && STILL_WAIT);
	if (token != 0xFE)
		return false; /* Function fails if invalid DataStart token or timeout */

	disk.multiread (buff, btr); /* Store trailing data to the buffer */
	disk.read (0xFF);
	disk.read (0xFF);
	return true; /* Function succeeded */
}

bool
xmit_datablock (const BYTE *buff, BYTE token)
{
	BYTE resp;

	if (!wait4ready (500))
		{
			return false; /* Wait for card ready */
		}

	disk.read (token); /* Send token */
	if (token != 0xFD)
		{ /* Send data if token is other than StopTran */
			disk.multiwrite (buff, 512); /* Data */
			/* Dummy CRC */
			disk.read (0xFF);
			disk.read (0xFF);

			resp = disk.read (0xFF); /* Receive data resp */
			if ((resp & 0x1F) != 0x05) /* Function fails if the data packet was not accepted */
				return false;
		}
	return true;
}

DSTATUS
disk_status (BYTE drv)
{
	return drv ? STA_NOINIT : MMCstat;
}

DRESULT
disk_read (BYTE drv, BYTE* buff, DWORD sector, UINT count)
{
	if (drv || !count)
		return RES_PARERR; /* Check parameter */
	if (MMCstat & STA_NOINIT)
		return RES_NOTRDY; /* Check if drive is ready */

	if (!(CardType & CT_BLOCK))
		sector *= 512; /* LBA ot BA conversion (byte addressing cards) */

	if (count == 1)
		{ /* Single sector read */
			if ((mmccmd (CMD17, sector) == 0) /* READ_SINGLE_BLOCK */
			&& rcvr_datablock (buff, 512))
				count = 0;
		}
	else
		{ /* Multiple sector read */
			if (mmccmd (CMD18, sector) == 0)
				{ /* READ_MULTIPLE_BLOCK */
					do
						{
							if (!rcvr_datablock (buff, 512))
								break;
							buff += 512;
						}
					while (--count);
					mmccmd (CMD12, 0); /* STOP_TRANSMISSION */
				}
		}
	deselect ();

	return count ? RES_ERROR : RES_OK; /* Return result */
}

DRESULT
disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
	if (drv || !count)
		return RES_PARERR; /* Check parameter */
	if (MMCstat & STA_NOINIT)
		return RES_NOTRDY; /* Check drive status */
	if (MMCstat & STA_PROTECT)
		return RES_WRPRT; /* Check write protect */

	if (!(CardType & CT_BLOCK))
		sector *= 512; /* LBA ==> BA conversion (byte addressing cards) */

	if (count == 1)
		{ /* Single sector write */
			if (0 == mmccmd (CMD24, sector))
				{
					/* WRITE_BLOCK */
					if (xmit_datablock (buff, 0xFE))
						{
							count = 0;
						}
				}
		}
	else
		{ /* Multiple sector write */
			if (CardType & CT_SDC)
				mmccmd (ACMD23, count); /* Predefine number of sectors */
			if (mmccmd (CMD25, sector) == 0)
				{ /* WRITE_MULTIPLE_BLOCK */
					do
						{
							if (!xmit_datablock (buff, 0xFC))
								break;
							buff += 512;
						}
					while (--count);
					if (!xmit_datablock (0, 0xFD)) /* STOP_TRAN token */
						count = 1;
				}
		}
	deselect ();

	return count ? RES_ERROR : RES_OK; /* Return result */
}

DRESULT
disk_ioctl (BYTE drv, BYTE cmd, void* buff)
{
	DRESULT res;
	BYTE n, csd[16];
	DWORD *dp, st, ed, csize;

	if (drv)
		return RES_PARERR; /* Check parameter */
	if (MMCstat & STA_NOINIT)
		return RES_NOTRDY; /* Check if drive is ready */

	res = RES_ERROR;

	switch (cmd)
		{
		case CTRL_SYNC: /* Wait for end of internal write process of the drive */
			if (select ())
				res = RES_OK;
			break;

		case GET_SECTOR_COUNT: /* Get drive capacity in unit of sector (DWORD) */
			if ((mmccmd (CMD9, 0) == 0) && rcvr_datablock (csd, 16))
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
					if (mmccmd (ACMD13, 0) == 0)
						{ /* Read SD status */
							mmccmd (0xFF, 0);
							if (rcvr_datablock (csd, 16))
								{ /* Read partial block */
									for (n = 64 - 16; n; n--)
										disk.read (0xFF); /* Purge trailing data */
									*(DWORD*) buff = 16UL << (csd[10] >> 4);
									res = RES_OK;
								}
						}
				}
			else
				{ /* SDC ver 1.XX or MMC */
					if ((mmccmd (CMD9, 0) == 0) && rcvr_datablock (csd, 16))
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
			if (!(CardType & CT_SDC))
				break; /* Check if the card is SDC */
			if (disk_ioctl (drv, MMC_GET_CSD, csd))
				break; /* Get CSD */
			if (!(csd[0] >> 6) && !(csd[10] & 0x40))
				break; /* Check if sector erase can be applied to the card */
			dp = (DWORD*) buff;
			st = dp[0];
			ed = dp[1]; /* Load sector block */
			if (!(CardType & CT_BLOCK))
				{
					st *= 512;
					ed *= 512;
				}
			if (mmccmd (CMD32, st) == 0 && mmccmd (CMD33, ed) == 0
			    && mmccmd (CMD38, 0) == 0 && wait4ready (30000)) /* Erase sector block */
				res = RES_OK; /* FatFs does not check result of this command */
			break;

		default:
			res = RES_PARERR;
		}

	deselect ();
	return res;
}

const char *
get_fresult (FRESULT r)
{
	switch (r)
		{
		case FR_OK:
			return "OK";
		case FR_DISK_ERR:
			return "DISK ERR";
		case FR_INT_ERR:
			return "INT ERR";
		case FR_NOT_READY:
			return "NOT READY";
		case FR_NO_FILE:
			return "NO FILE";
		case FR_NO_PATH:
			return "NO PATH";
		case FR_INVALID_NAME:
			return "INVALID NAME";
		case FR_DENIED:
			return "DENIED";
		case FR_EXIST:
			return "EXIST";
		case FR_INVALID_OBJECT:
			return "INVALID OBJECT";
		case FR_WRITE_PROTECTED:
			return "WRITE PROTECTED";
		case FR_INVALID_DRIVE:
			return "INVALID DRIVE";
		case FR_NOT_ENABLED:
			return "NOT ENABLED";
		case FR_NO_FILESYSTEM:
			return "NO FILESYSTEM";
		case FR_MKFS_ABORTED:
			return "MKFS ABORTED";
		case FR_TIMEOUT:
			return "TIMEOUT";
		case FR_LOCKED:
			return "LOCKED";
		case FR_NOT_ENOUGH_CORE:
			return "NOT ENOUGH CORE";
		case FR_TOO_MANY_OPEN_FILES:
			return "TOO MANY OPEN FILES";
		case FR_INVALID_PARAMETER:
			return "INVALID PARAMETER";
		default:
			return "Unknown result";
		}
}

const char *
get_dresult (DRESULT r)
{
	switch(r)
	{
		case RES_OK:
			return "OK";
		case RES_ERROR:
			return "ERROR";
		case RES_WRPRT:
			return "WRPRT";
		case RES_NOTRDY:
			return "NOTRDY";
		case RES_PARERR:
			return "PARERR";
		default:
			return "Unknown error";
	}
}

void
disk_test (void)
{
	Uart out (1, CONSOLE_SPEED);
	Console con (&out);
	con.print ("FatFs test started\r\n");
	disk = SPI::Spi (1);
	//disk.go16bit();
	word res = disk.read(0xAABB);
	con.xprintf("Result: 0x%X\r\n", res);
}
