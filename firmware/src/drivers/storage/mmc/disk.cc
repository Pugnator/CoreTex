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

DWORD get_fattime (void)
{
	return 0;
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

	if( SD_RESPONSE_FAILURE == SD_GoIdleState())
	{
		LOGPRINT("SD init failed\r\n");
		MMCstat = STA_NOINIT;
	}
	else
	{
		LOGPRINT("SD init OK\r\n");
		disk.highspeed();
		MMCstat &= ~STA_NOINIT;
	}
	return MMCstat;
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
	if(SD_RESPONSE_NO_ERROR == SD_ReadBlock(buff, sector * _MIN_SS, count * _MIN_SS))
	{
		return RES_OK; /* Return result */
	}
	else
	{
		return RES_ERROR; /* Return result */
	}
}

DRESULT
disk_write (BYTE drv, const BYTE* buff, DWORD sector, UINT count)
{
	DBGPRINT("disk_write: Drive=%u, sector=%u, count=%u\r\n", drv, sector, count);
	if(SD_RESPONSE_NO_ERROR == SD_WriteBlock(buff, sector * _MIN_SS, count * _MIN_SS))
	{
		return RES_OK; /* Return result */
	}
	else
	{
		return RES_ERROR; /* Return result */
	}
}

DRESULT
disk_ioctl (BYTE drv, BYTE cmd, void* buff)
{
	LOGPRINT("disk_ioctl drive=%u, cmd=%u\r\n", drv, cmd);
	if (drv)
		return RES_PARERR; /* Check parameter */
	if (MMCstat & STA_NOINIT)
		return RES_NOTRDY; /* Check if drive is ready */

	DWORD sector = 0;
	DWORD bs = 0;

	switch (cmd)
	{
	case CTRL_SYNC: /* Wait for end of internal write process of the drive */
		return RES_OK;

	case GET_SECTOR_COUNT: /* Get drive capacity in unit of sector (DWORD) */
		sector = get_card_capacity() / get_card_block_size();
		*(DWORD*) buff = sector;
		LOGPRINT("sector = %u\r\n", *(DWORD*) buff);
		return RES_OK;

	case GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
		bs = get_card_block_size();
		*(DWORD*) buff = bs;
		LOGPRINT("bs = %u\r\n", *(DWORD*) buff);
		return RES_OK;

	case CTRL_TRIM: /* Erase a block of sectors (used when _USE_ERASE == 1) */
		return RES_OK;
	default:
		return RES_PARERR;
	}
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

uint32_t get_card_capacity()
{
	SD_CSD SD_csd = {};
	if (SD_RESPONSE_NO_ERROR == SD_GetCSDRegister(&SD_csd))
	{
		uint32_t CardCapacity = (SD_csd.DeviceSize + 1) ;
		CardCapacity *= (1 << (SD_csd.DeviceSizeMul + 2));
		CardCapacity = (SD_csd.DeviceSize + 1) ;
		CardCapacity *= (1 << (SD_csd.DeviceSizeMul + 2));
		uint32_t CardBlockSize = 1 << (SD_csd.RdBlockLen);
		CardCapacity *= CardBlockSize;
		LOGPRINT("CardCapacity = %u\r\n", CardCapacity);
		return CardCapacity;
	}
	else
	{
		return 0;
	}
}

uint32_t get_card_block_size()
{
	SD_CSD SD_csd = {};
		if (SD_RESPONSE_NO_ERROR == SD_GetCSDRegister(&SD_csd))
		{
			return 1 << (SD_csd.RdBlockLen);
		}
		else
		{
			return 0;
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
	disk.go8bit();
  disk.lowspeed();
 	for (int i = 0; i <=100; i++)
 	{
 	    /*!< Send dummy byte 0xFF */
 	 disk.read();
 	}
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
  SD_Error rvalue = SD_RESPONSE_FAILURE;
  uint8_t CSD_Tab[16];

  /*!< Send CMD9 (CSD register) or CMD10(CSD register) */
  SD_SendCmd(CMD9, 0, 0xFF);
  /*!< Wait for response in the R1 format (0x00 is no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    if (!SD_GetResponse(0xFE))
    {
      for (int i = 0; i < 16; i++)
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
  /*!< Return the reponse */
  return rvalue;
}

uint8_t SD_GetDataResponse(void)
{
  uint32_t i = 0;
  uint8_t response, rvalue;

  while (i <= 64)
  {
    /*!< Read resonse */
    response = disk.read();
    /*!< Mask unused bits */
    response &= 0x1F;
    switch (response)
    {
      case SD_DATA_OK:
      {
        rvalue = SD_DATA_OK;
        break;
      }
      case SD_DATA_CRC_ERROR:
        return SD_DATA_CRC_ERROR;
      case SD_DATA_WRITE_ERROR:
        return SD_DATA_WRITE_ERROR;
      default:
      {
        rvalue = SD_DATA_OTHER_ERROR;
        break;
      }
    }
    /*!< Exit loop in case of data ok */
    if (rvalue == SD_DATA_OK)
      break;
    /*!< Increment loop counter */
    i++;
  }

  /*!< Wait null data */
  while (disk.read() == 0);

  /*!< Return response */
  return response;
}

SD_Error SD_WriteBlock(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize)
{
  uint32_t i = 0;
  SD_Error rvalue = SD_RESPONSE_FAILURE;


  /*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write multiple block */
  SD_SendCmd(CMD24, WriteAddr, 0xFF);

  /*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
  if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
  {
    /*!< Send a dummy byte */
  	disk.read();

    /*!< Send the data token to signify the start of the data */
    disk.read(0xFE);

    /*!< Write the block data to SD : write count data by block */
    for (i = 0; i < BlockSize; i++)
    {
      /*!< Send the pointed byte */
    	disk.read(*pBuffer);
      /*!< Point to the next location where the byte read will be saved */
      pBuffer++;
    }
    /*!< Put CRC bytes (not really needed by us, but required by SD) */
    disk.read();
    disk.read();

    /*!< Read data response */
    if (SD_GetDataResponse() == SD_DATA_OK)
    {
      rvalue = SD_RESPONSE_NO_ERROR;
    }
  }
  /*!< SD chip select high */
  /*!< Send dummy byte: 8 Clock pulses of delay */
  disk.read();

  if(rvalue == SD_RESPONSE_NO_ERROR)
   {
    	LOGPRINT("Successfully wrote the block\r\n");
   }
   else
    {
   	LOGPRINT("Failed to write the block\r\n");
    }

  /*!< Returns the reponse */
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

  if(rvalue == SD_RESPONSE_NO_ERROR)
  {
   	LOGPRINT("Successfully read the block\r\n");
  }
  else
   {
  	LOGPRINT("Failed to read the block\r\n");
   }

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
	LOGPRINT("FatFs test started\r\n");
	disk = SPI::Spi (1);
	FRESULT r;
	FATFS fs;
	r = f_mount(&fs, "0:", 1);
	LOGPRINT("f_mount? %s\r\n", get_fresult(r));
	//r = f_mkfs("0:", 0, 0);
	//LOGPRINT("f_mount? %s\r\n", get_fresult(r));
	FIL test;
	r = f_open(&test, "test.txt", FA_OPEN_ALWAYS);
	LOGPRINT("f_open? %s\r\n", get_fresult(r));
	f_close(&test);
	LOGPRINT("f_close? %s\r\n", get_fresult(r));
}

