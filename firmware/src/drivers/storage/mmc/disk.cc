#include <core/spi.hpp>
#include <global.hpp>
#include <common.hpp>
#include <log.hpp>
#include <drivers/storage/fatfs/ff.h>
#include <drivers/storage/fatfs/diskio.h>
#include <drivers/storage/disk.hpp>
#include <drivers/console.hpp>

static SPI::Spi disk (1);
static DSTATUS MMCstat; /* Physical drive status */
static BYTE CardType;

typedef enum
{
/**
  * @brief  SD reponses and error flags
  */
  SD_RESPONSE_NO_ERROR      = (0x00),
  SD_IN_IDLE_STATE          = (0x01),
  SD_ERASE_RESET            = (0x02),
  SD_ILLEGAL_COMMAND        = (0x04),
  SD_COM_CRC_ERROR          = (0x08),
  SD_ERASE_SEQUENCE_ERROR   = (0x10),
  SD_ADDRESS_ERROR          = (0x20),
  SD_PARAMETER_ERROR        = (0x40),
  SD_RESPONSE_FAILURE       = (0xFF),

/**
  * @brief  Data response error
  */
  SD_DATA_OK                = (0x05),
  SD_DATA_CRC_ERROR         = (0x0B),
  SD_DATA_WRITE_ERROR       = (0x0D),
  SD_DATA_OTHER_ERROR       = (0xFF)
} SD_Error;

typedef struct
{
  __IO uint8_t  CSDStruct;            /*!< CSD structure */
  __IO uint8_t  SysSpecVersion;       /*!< System specification version */
  __IO uint8_t  Reserved1;            /*!< Reserved */
  __IO uint8_t  TAAC;                 /*!< Data read access-time 1 */
  __IO uint8_t  NSAC;                 /*!< Data read access-time 2 in CLK cycles */
  __IO uint8_t  MaxBusClkFrec;        /*!< Max. bus clock frequency */
  __IO uint16_t CardComdClasses;      /*!< Card command classes */
  __IO uint8_t  RdBlockLen;           /*!< Max. read data block length */
  __IO uint8_t  PartBlockRead;        /*!< Partial blocks for read allowed */
  __IO uint8_t  WrBlockMisalign;      /*!< Write block misalignment */
  __IO uint8_t  RdBlockMisalign;      /*!< Read block misalignment */
  __IO uint8_t  DSRImpl;              /*!< DSR implemented */
  __IO uint8_t  Reserved2;            /*!< Reserved */
  __IO uint32_t DeviceSize;           /*!< Device Size */
  __IO uint8_t  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min */
  __IO uint8_t  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max */
  __IO uint8_t  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min */
  __IO uint8_t  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max */
  __IO uint8_t  DeviceSizeMul;        /*!< Device size multiplier */
  __IO uint8_t  EraseGrSize;          /*!< Erase group size */
  __IO uint8_t  EraseGrMul;           /*!< Erase group size multiplier */
  __IO uint8_t  WrProtectGrSize;      /*!< Write protect group size */
  __IO uint8_t  WrProtectGrEnable;    /*!< Write protect group enable */
  __IO uint8_t  ManDeflECC;           /*!< Manufacturer default ECC */
  __IO uint8_t  WrSpeedFact;          /*!< Write speed factor */
  __IO uint8_t  MaxWrBlockLen;        /*!< Max. write data block length */
  __IO uint8_t  WriteBlockPaPartial;  /*!< Partial blocks for write allowed */
  __IO uint8_t  Reserved3;            /*!< Reserded */
  __IO uint8_t  ContentProtectAppli;  /*!< Content protection application */
  __IO uint8_t  FileFormatGrouop;     /*!< File format group */
  __IO uint8_t  CopyFlag;             /*!< Copy flag (OTP) */
  __IO uint8_t  PermWrProtect;        /*!< Permanent write protection */
  __IO uint8_t  TempWrProtect;        /*!< Temporary write protection */
  __IO uint8_t  FileFormat;           /*!< File Format */
  __IO uint8_t  ECC;                  /*!< ECC code */
  __IO uint8_t  CSD_CRC;              /*!< CSD CRC */
  __IO uint8_t  Reserved4;            /*!< always 1*/
} SD_CSD;

typedef struct
{
  __IO uint8_t  ManufacturerID;       /*!< ManufacturerID */
  __IO uint16_t OEM_AppliID;          /*!< OEM/Application ID */
  __IO uint32_t ProdName1;            /*!< Product Name part1 */
  __IO uint8_t  ProdName2;            /*!< Product Name part2*/
  __IO uint8_t  ProdRev;              /*!< Product Revision */
  __IO uint32_t ProdSN;               /*!< Product Serial Number */
  __IO uint8_t  Reserved1;            /*!< Reserved1 */
  __IO uint16_t ManufactDate;         /*!< Manufacturing Date */
  __IO uint8_t  CID_CRC;              /*!< CID CRC */
  __IO uint8_t  Reserved2;            /*!< always 1 */
} SD_CID;

DWORD get_fattime (void)
{
	return 0;
}

bool
wait4ready (word how_long)
{
	DBGPRINT("wait4ready %ums\r\n", how_long);
	uint16_t d = 0;	
	WAIT_FOR (how_long);
	do
		{
			d = disk.read (0xFF);
		}
	while (d != 0xFF && STILL_WAIT); /* Wait for card goes ready or timeout */

	return d == 0xFF;
}

void
deselect (void)
{
	DBGPRINT("SD deselect\r\n");
	PIN_HI (SPI1NSS_PIN);
	/* Dummy clock (force DO hi-z for multiple slave SPI) */
	//disk.read (0xff);
}

bool
select (void)
{	
	DBGPRINT("SD select\r\n");
	PIN_LOW (SPI1NSS_PIN);
	/* Dummy clock (force DO enabled) */
	//disk.read (0xff);
	if (wait4ready(500))
	{
		DBGPRINT("SD selected\r\n");
		return true;      /* Wait for card ready */
	}
	DBGPRINT("SD selection failed\r\n");
	deselect();
	return false;
}

const char *cmd2str (uint8_t command)
{
	switch (command)
	{
		case 0:
			return "CMD0";
		case 1:
			return "CMD1";
		case 0x80+41:
			return "ACMD41";
		case 8:
			return "CMD8";
		case 9:
			return "CMD9";
		case 10:
			return "CMD10";
		case 12:
			return "CMD12";
		case 0x80+13:
			return "ACMD13";
		case 16:   
			return "CMD16";
		case 17:    
			return "CMD17";
		case 18:
			return "CMD18";
		case 0x80+23:
			return "CMD23";
		case 24:   
			return "ACMD23";
		case 25:   
			return "CMD24";
		case 32:   
			return "CMD25";
		case 33:   
			return "CMD33";
		case 38:   
			return "CMD38";
		case 55:   
			return "CMD55";
		case 58:   
			return "CMD58";
		default:
			return "Unknown command";
	}
}

uint16_t SD_SendCmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc = 0)
{
  uint32_t i = 0x00;
  switch (Cmd)
  {
  	case CMD0:
  		Crc = 0x95;
  		break;
  	case CMD8:
  		Crc = 0x87;
  		break;
  }

  uint8_t Frame[6];

  Frame[0] = (Cmd | 0x40); /*!< Construct byte 1 */
  Frame[1] = (uint8_t)(Arg >> 24); /*!< Construct byte 2 */
  Frame[2] = (uint8_t)(Arg >> 16); /*!< Construct byte 3 */
  Frame[3] = (uint8_t)(Arg >> 8); /*!< Construct byte 4 */
  Frame[4] = (uint8_t)(Arg); /*!< Construct byte 5 */
  Frame[5] = (Crc); /*!< Construct CRC: byte 6 */

  uint16_t res = 0;

  for (i = 0; i < 6; i++)
  {
    res = disk.read(Frame[i]); /*!< Send the Cmd bytes */
  }
  return res;
}

DSTATUS
disk_initialize (BYTE drv)
{
	DBGPRINT("disk_initialize: Drive=%u\r\n", drv);
	disk.lowspeed();
	disk.go8bit();
	BYTE cmd, ty, ocr[4];
	if (drv)
	{
		DBGPRINT("disk_initialize: STA_NOINIT\r\n");
		return STA_NOINIT; /* Supports only drive 0 */
	}

	if (MMCstat & STA_NODISK)
	{
		DBGPRINT("disk_initialize: MMCstat=%u\r\n", MMCstat);
		return MMCstat; /* Is card existing in the soket? */
	}

	for (word n = 20; n; n--)
	{		
		disk.read (0xFF); /* Send 80 dummy clocks */
	}

	ty = 0;	
	if (SD_SendCmd (CMD0, 0) == 1)
		{ /* Put the card SPI/Idle state */
			WAIT_FOR (5000); /* Initialization timeout = 1 sec */
			if (SD_SendCmd (CMD8, 0x1AA) == 1)
				{ /* SDv2? */
					for (word n = 0; n < 4; n++)
					{						
						ocr[n] = disk.read (0xFF); /* Get 32 bit return value of R7 resp */
						DBGPRINT("disk_initialize: Get 32 bit return value of R7 resp, %u [%u]\r\n", ocr[n], n);
					}
					if (ocr[2] == 0x01 && ocr[3] == 0xAA)
						{ 
							DBGPRINT("disk_initialize: ocr[2] == 0x01 && ocr[3] == 0xAA\r\n");
							/* Is the card supports vcc of 2.7-3.6V? */
							while (STILL_WAIT && SD_SendCmd (ACMD41, 1UL << 30))
								; /* Wait for end of initialization with ACMD41(HCS) */
							if (STILL_WAIT && SD_SendCmd (CMD58, 0) == 0)
								{ /* Check CCS bit in the OCR */
									for (word n = 0; n < 4; n++)
										ocr[n] = SD_SendCmd (0xFF, 0);
									ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2; /* Card id SDv2 */
								}
						}
				}
			else
				{ /* Not SDv2 card */
					if (SD_SendCmd (ACMD41, 0) <= 1)
						{ /* SDv1 or MMC? */
							ty = CT_SD1;
							cmd = ACMD41; /* SDv1 (ACMD41(0)) */
						}
					else
						{
							ty = CT_MMC;
							cmd = CMD1; /* MMCv3 (CMD1(0)) */
						}
					while (STILL_WAIT && SD_SendCmd (cmd, 0))
						; /* Wait for end of initialization */
					if (!STILL_WAIT || SD_SendCmd (CMD16, 512) != 0) /* Set block length: 512 */
						ty = 0;
				}
		}
	CardType = ty; /* Card type */
	deselect ();

	if (ty)
		{
			/* Set fast clock */
			disk.highspeed ();
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
	DBGPRINT("rcvr_datablock: Blocks=%u\r\n", btr);
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
	DBGPRINT("xmit_datablock: token=%u\r\n", token);
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
	DBGPRINT("disk_read: Drive=%u, sector=%u, count=%u\r\n", drv, sector, count);
	if (drv || !count)
		return RES_PARERR; /* Check parameter */
	if (MMCstat & STA_NOINIT)
		return RES_NOTRDY; /* Check if drive is ready */

	if (!(CardType & CT_BLOCK))
		sector *= 512; /* LBA ot BA conversion (byte addressing cards) */

	if (count == 1)
		{ /* Single sector read */
			if ((SD_SendCmd (CMD17, sector) == 0) /* READ_SINGLE_BLOCK */
			&& rcvr_datablock (buff, 512))
				count = 0;
		}
	else
		{ /* Multiple sector read */
			if (SD_SendCmd (CMD18, sector) == 0)
				{ /* READ_MULTIPLE_BLOCK */
					do
						{
							if (!rcvr_datablock (buff, 512))
								break;
							buff += 512;
						}
					while (--count);
					SD_SendCmd (CMD12, 0); /* STOP_TRANSMISSION */
				}
		}
	deselect ();

	return count ? RES_ERROR : RES_OK; /* Return result */
}

DRESULT
disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
	DBGPRINT("disk_write: Drive=%u, sector=%u, count=%u\r\n", drv, sector, count);
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
			if (0 == SD_SendCmd (CMD24, sector))
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
				SD_SendCmd (ACMD23, count); /* Predefine number of sectors */
			if (SD_SendCmd (CMD25, sector) == 0)
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
			if ((SD_SendCmd (CMD9, 0) == 0) && rcvr_datablock (csd, 16))
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
					if (SD_SendCmd (ACMD13, 0) == 0)
						{ /* Read SD status */
							SD_SendCmd (0xFF, 0);
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
					if ((SD_SendCmd (CMD9, 0) == 0) && rcvr_datablock (csd, 16))
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
			if (SD_SendCmd (CMD32, st) == 0 && SD_SendCmd (CMD33, ed) == 0
			    && SD_SendCmd (CMD38, 0) == 0 && wait4ready (30000)) /* Erase sector block */
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

SD_Error SD_GetResponse(uint8_t Response)
{
  uint32_t Count = 0xFFF;
  /*!< Check if response is got or a timeout is happen */
  while ((disk.read() != Response) && Count)
  {
    Count--;
  }
  if (Count == 0)
  {
    /*!< After time out */
    return SD_RESPONSE_FAILURE;
  }
  else
  {
    /*!< Right response got */
    return SD_RESPONSE_NO_ERROR;
  }
}

SD_Error SD_GoIdleState(void)
{
  /*!< Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode */
  SD_SendCmd(CMD0, 0, 0x95);

  /*!< Wait for In Idle State Response (R1 Format) equal to 0x01 */
  if (SD_GetResponse(SD_IN_IDLE_STATE))
  {
    /*!< No Idle State Response: return response failue */
    return SD_RESPONSE_FAILURE;
  }
  /*----------Activates the card initialization process-----------*/
  do
  {
    /*!< SD chip select high */
    /*!< Send Dummy byte 0xFF */
    disk.read();

    /*!< Send CMD1 (Activates the card process) until response equal to 0x0 */
    SD_SendCmd(CMD1, 0, 0xFF);
    /*!< Wait for no error Response (R1 Format) equal to 0x00 */
  }
  while (SD_GetResponse(SD_RESPONSE_NO_ERROR));

  /*!< SD chip select high */

  /*!< Send dummy byte 0xFF */
  disk.read();

  return SD_RESPONSE_NO_ERROR;
}

SD_Error SD_GetCIDRegister(SD_CID* SD_cid)
{
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  uint8_t CID_Tab[16];

  /*!< Send CMD10 (CID register) */
  SD_SendCmd(CMD10, 0, 0xFF);

  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(0xFE))
    {
      /*!< Store CID register value on CID_Tab */
      for (i = 0; i < 16; i++)
      {
        CID_Tab[i] = disk.read();
      }
    }
    /*!< Get CRC bytes (not really needed by us, but required by SD) */
    disk.read();
    disk.read();
    /*!< Set response value to success */
    rvalue = SD_RESPONSE_NO_ERROR;
  }

  /*!< Send dummy byte: 8 Clock pulses of delay */
  disk.read();

  /*!< Byte 0 */
  SD_cid->ManufacturerID = CID_Tab[0];

  /*!< Byte 1 */
  SD_cid->OEM_AppliID = CID_Tab[1] << 8;

  /*!< Byte 2 */
  SD_cid->OEM_AppliID |= CID_Tab[2];

  /*!< Byte 3 */
  SD_cid->ProdName1 = CID_Tab[3] << 24;

  /*!< Byte 4 */
  SD_cid->ProdName1 |= CID_Tab[4] << 16;

  /*!< Byte 5 */
  SD_cid->ProdName1 |= CID_Tab[5] << 8;

  /*!< Byte 6 */
  SD_cid->ProdName1 |= CID_Tab[6];

  /*!< Byte 7 */
  SD_cid->ProdName2 = CID_Tab[7];

  /*!< Byte 8 */
  SD_cid->ProdRev = CID_Tab[8];

  /*!< Byte 9 */
  SD_cid->ProdSN = CID_Tab[9] << 24;

  /*!< Byte 10 */
  SD_cid->ProdSN |= CID_Tab[10] << 16;

  /*!< Byte 11 */
  SD_cid->ProdSN |= CID_Tab[11] << 8;

  /*!< Byte 12 */
  SD_cid->ProdSN |= CID_Tab[12];

  /*!< Byte 13 */
  SD_cid->Reserved1 |= (CID_Tab[13] & 0xF0) >> 4;
  SD_cid->ManufactDate = (CID_Tab[13] & 0x0F) << 8;

  /*!< Byte 14 */
  SD_cid->ManufactDate |= CID_Tab[14];

  /*!< Byte 15 */
  SD_cid->CID_CRC = (CID_Tab[15] & 0xFE) >> 1;
  SD_cid->Reserved2 = 1;

  /*!< Return the reponse */
  return rvalue;
}

SD_Error SD_GetCSDRegister(SD_CSD* SD_csd)
{
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  uint8_t CSD_Tab[16];

  /*!< Send CMD9 (CSD register) or CMD10(CSD register) */
  SD_SendCmd(CMD9, 0, 0xFF);
  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(0xFE))
    {
      for (i = 0; i < 16; i++)
      {
        /*!< Store CSD register value on CSD_Tab */
        CSD_Tab[i] = disk.read();
      }
    }
    /*!< Get CRC bytes (not really needed by us, but required by SD) */
    disk.read(0xFF);
    disk.read(0xFF);
    /*!< Set response value to success */
    rvalue = SD_RESPONSE_NO_ERROR;
  }

  /*!< Send dummy byte: 8 Clock pulses of delay */
  disk.read();

  /*!< Byte 0 */
  SD_csd->CSDStruct = (CSD_Tab[0] & 0xC0) >> 6;
  SD_csd->SysSpecVersion = (CSD_Tab[0] & 0x3C) >> 2;
  SD_csd->Reserved1 = CSD_Tab[0] & 0x03;

  /*!< Byte 1 */
  SD_csd->TAAC = CSD_Tab[1];

  /*!< Byte 2 */
  SD_csd->NSAC = CSD_Tab[2];

  /*!< Byte 3 */
  SD_csd->MaxBusClkFrec = CSD_Tab[3];

  /*!< Byte 4 */
  SD_csd->CardComdClasses = CSD_Tab[4] << 4;

  /*!< Byte 5 */
  SD_csd->CardComdClasses |= (CSD_Tab[5] & 0xF0) >> 4;
  SD_csd->RdBlockLen = CSD_Tab[5] & 0x0F;

  /*!< Byte 6 */
  SD_csd->PartBlockRead = (CSD_Tab[6] & 0x80) >> 7;
  SD_csd->WrBlockMisalign = (CSD_Tab[6] & 0x40) >> 6;
  SD_csd->RdBlockMisalign = (CSD_Tab[6] & 0x20) >> 5;
  SD_csd->DSRImpl = (CSD_Tab[6] & 0x10) >> 4;
  SD_csd->Reserved2 = 0; /*!< Reserved */

  SD_csd->DeviceSize = (CSD_Tab[6] & 0x03) << 10;

  /*!< Byte 7 */
  SD_csd->DeviceSize |= (CSD_Tab[7]) << 2;

  /*!< Byte 8 */
  SD_csd->DeviceSize |= (CSD_Tab[8] & 0xC0) >> 6;

  SD_csd->MaxRdCurrentVDDMin = (CSD_Tab[8] & 0x38) >> 3;
  SD_csd->MaxRdCurrentVDDMax = (CSD_Tab[8] & 0x07);

  /*!< Byte 9 */
  SD_csd->MaxWrCurrentVDDMin = (CSD_Tab[9] & 0xE0) >> 5;
  SD_csd->MaxWrCurrentVDDMax = (CSD_Tab[9] & 0x1C) >> 2;
  SD_csd->DeviceSizeMul = (CSD_Tab[9] & 0x03) << 1;
  /*!< Byte 10 */
  SD_csd->DeviceSizeMul |= (CSD_Tab[10] & 0x80) >> 7;

  SD_csd->EraseGrSize = (CSD_Tab[10] & 0x40) >> 6;
  SD_csd->EraseGrMul = (CSD_Tab[10] & 0x3F) << 1;

  /*!< Byte 11 */
  SD_csd->EraseGrMul |= (CSD_Tab[11] & 0x80) >> 7;
  SD_csd->WrProtectGrSize = (CSD_Tab[11] & 0x7F);

  /*!< Byte 12 */
  SD_csd->WrProtectGrEnable = (CSD_Tab[12] & 0x80) >> 7;
  SD_csd->ManDeflECC = (CSD_Tab[12] & 0x60) >> 5;
  SD_csd->WrSpeedFact = (CSD_Tab[12] & 0x1C) >> 2;
  SD_csd->MaxWrBlockLen = (CSD_Tab[12] & 0x03) << 2;

  /*!< Byte 13 */
  SD_csd->MaxWrBlockLen |= (CSD_Tab[13] & 0xC0) >> 6;
  SD_csd->WriteBlockPaPartial = (CSD_Tab[13] & 0x20) >> 5;
  SD_csd->Reserved3 = 0;
  SD_csd->ContentProtectAppli = (CSD_Tab[13] & 0x01);

  /*!< Byte 14 */
  SD_csd->FileFormatGrouop = (CSD_Tab[14] & 0x80) >> 7;
  SD_csd->CopyFlag = (CSD_Tab[14] & 0x40) >> 6;
  SD_csd->PermWrProtect = (CSD_Tab[14] & 0x20) >> 5;
  SD_csd->TempWrProtect = (CSD_Tab[14] & 0x10) >> 4;
  SD_csd->FileFormat = (CSD_Tab[14] & 0x0C) >> 2;
  SD_csd->ECC = (CSD_Tab[14] & 0x03);

  /*!< Byte 15 */
  SD_csd->CSD_CRC = (CSD_Tab[15] & 0xFE) >> 1;
  SD_csd->Reserved4 = 1;

  uint32_t CardCapacity = 0;
  uint32_t CardBlockSize = 0;
  CardCapacity = (SD_csd->DeviceSize + 1) ;
  CardCapacity *= (1 << (SD_csd->DeviceSizeMul + 2));
  CardBlockSize = 1 << (SD_csd->RdBlockLen);
  CardCapacity *= CardBlockSize;
  LOGPRINT("CardCapacity = %u, CardBlockSize = %u\r\n", CardCapacity, CardBlockSize);
  /*!< Return the reponse */
  return rvalue;
}

SD_Error SD_ReadBlock(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize)
{
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;

  /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
  SD_SendCmd(CMD17, ReadAddr, 0xFF);

  /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    /*!< Now look for the data token to signify the start of the data */
    if (!SD_GetResponse(0xFE))
    {
      /*!< Read the SD block data : read NumByteToRead data */
      for (i = 0; i < BlockSize; i++)
      {
        /*!< Save the received data */
        *pBuffer = disk.read();

        /*!< Point to the next location where the byte read will be saved */
        pBuffer++;
      }
      /*!< Get CRC bytes (not really needed by us, but required by SD) */
      disk.read();
      disk.read();
      /*!< Set response value to success */
      rvalue = SD_RESPONSE_NO_ERROR;
    }
  }

  /*!< Send dummy byte: 8 Clock pulses of delay */
  disk.read();

  /*!< Returns the reponse */
  return rvalue;
}

uint16_t SD_GetStatus(void)
{
  uint16_t Status = 0;

  /*!< Send CMD13 (SD_SEND_STATUS) to get SD status */
  SD_SendCmd(ACMD13, 0, 0xFF);

  Status = disk.read();
  Status |= (uint16_t)(disk.read() << 8);

  /*!< Send dummy byte 0xFF */
  disk.read();

  return Status;
}

void
disk_test (void)
{	
	__dbg_out->cls();
	LOGPRINT("FatFs test started\r\n");
	disk = SPI::Spi (1);
	disk.go16bit();
	disk.lowspeed();
	for (int i = 0; i <=100; i++)
	  {
	    /*!< Send dummy byte 0xFF */
			 disk.read();
	  }
	if( SD_RESPONSE_FAILURE == SD_GoIdleState())
		{
			LOGPRINT("SD init failed\r\n");
			return;
		}
	LOGPRINT("Inited\r\n");
	uint8_t buf[512]={};
	SD_Error err = SD_ReadBlock(buf, 1, 512);
	if(SD_RESPONSE_NO_ERROR == err)
		{
			LOGPRINT("SD_RESPONSE_NO_ERROR\r\n");
		}
	else if (SD_RESPONSE_FAILURE == err)
		{
			LOGPRINT("SD_RESPONSE_FAILURE\r\n");
		}



	/*DRESULT r = disk_write(0, buf, 0, 1);
	DBGPRINT("%s\r\n",get_dresult(r));
	memset(buf, 0, sizeof buf);
	r = disk_read(0, buf, 0, 1);
	DBGPRINT("%s\r\n", get_dresult(r));	*/
	//r = disk_read(0, buf, 0, 1);
	//__dbg_out->print(get_dresult(r));
	//__dbg_out->put_dump(buf, 0, 512, 1);

}
