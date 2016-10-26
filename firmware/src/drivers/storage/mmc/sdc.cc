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
#include <core/io_macro.hpp>
#include <core/stm32f10x.hpp>
#include <global.hpp>
#include <core/spi.hpp>
#include <global.hpp>
#include <common.hpp>
#include <log.hpp>
#include <drivers/console.hpp>
#include <drivers/storage/sdc.hpp>

namespace SDC
{
/* MMC/SD command */
enum
{
	CMD0=    0,                     /* GO_IDLE_STATE */
	CMD1=    1,                     /* SEND_OP_COND (MMC) */
	ACMD41=  0x80+41,       /* SEND_OP_COND (SDC) */
	CMD8=    8,                     /* SEND_IF_COND */
	CMD9=    9,                     /* SEND_CSD */
	CMD10=   10,            /* SEND_CID */
	CMD12=   12,            /* STOP_TRANSMISSION */
	ACMD13=  0x80+13,       /* SD_STATUS (SDC) */
	CMD16=   16,            /* SET_BLOCKLEN */
	CMD17=   17,            /* READ_SINGLE_BLOCK */
	CMD18=   18,            /* READ_MULTIPLE_BLOCK */
	CMD23=   23,            /* SET_BLOCK_COUNT (MMC) */
	ACMD23=  0x80+23,       /* SET_WR_BLK_ERASE_COUNT (SDC) */
	CMD24=   24,            /* WRITE_BLOCK */
	CMD25=   25,            /* WRITE_MULTIPLE_BLOCK */
	CMD32=   32,            /* ERASE_ER_BLK_START */
	CMD33=   33,            /* ERASE_ER_BLK_END */
	CMD38=   38,            /* ERASE */
	CMD55=   55,            /* APP_CMD */
	CMD58=   58,            /* READ_OCR */
}SDCCMD;

const char *Sdc::cmd2str (uint8_t command)
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

uint16_t Sdc::cmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc = 0)
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
		res = read(Frame[i]); /*!< Send the Cmd bytes */
	}
	return res;
}

uint32_t Sdc::get_card_capacity()
{
	if (card_capacity)
	{
		return card_capacity;
	}
	SD_CSD SD_csd = {};
	if (SD_RESPONSE_NO_ERROR == get_CSD(&SD_csd))
	{
		uint32_t card_capacity = (SD_csd.DeviceSize + 1) ;
		card_capacity *= (1 << (SD_csd.DeviceSizeMul + 2));
		card_capacity = (SD_csd.DeviceSize + 1) ;
		card_capacity *= (1 << (SD_csd.DeviceSizeMul + 2));
		card_block_size = 1 << (SD_csd.RdBlockLen);
		card_capacity *= card_block_size;
		LOGPRINT("CardCapacity = %u\r\n", card_capacity);
		return card_capacity;
	}
	else
	{
		return 0;
	}
}

uint32_t Sdc::get_card_block_size()
{
	if (card_block_size)
	{
		return card_block_size;
	}
	SD_CSD SD_csd = {};
	if (SD_RESPONSE_NO_ERROR == get_CSD(&SD_csd))
	{
		card_block_size = 1 << (SD_csd.RdBlockLen);
		return card_block_size;
	}
	else
	{
		return 0;
	}
}

SDC_Error Sdc::get_response(uint8_t Response)
{
	ok = false;
	uint32_t Count = 0xFFF;
	/*!< Check if response is got or a timeout is happen */
	while ((read() != Response) && Count)
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
		ok = true;
		return SD_RESPONSE_NO_ERROR;
	}
}

SDC_Error Sdc::initialize(void)
{
	ok = false;
	go8bit();
	lowspeed();
	for (int i = 0; i <=100; i++)
	{
		/*!< Send dummy byte 0xFF */
		read();
	}
	/*!< Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode */
	cmd(CMD0, 0, 0x95);

	/*!< Wait for In Idle State Response (R1 Format) equal to 0x01 */
	if (get_response(SD_IN_IDLE_STATE))
	{
		/*!< No Idle State Response: return response failue */
		return SD_RESPONSE_FAILURE;
	}
	/*----------Activates the card initialization process-----------*/
	do
	{
		/*!< SD chip select high */
		/*!< Send Dummy byte 0xFF */
		read();

		/*!< Send CMD1 (Activates the card process) until response equal to 0x0 */
		cmd(CMD1, 0, 0xFF);
		/*!< Wait for no error Response (R1 Format) equal to 0x00 */
	}
	while (get_response(SD_RESPONSE_NO_ERROR));

	/*!< SD chip select high */

	/*!< Send dummy byte 0xFF */
	read();
	ok = true;
	return SD_RESPONSE_NO_ERROR;
}

SDC_Error Sdc::get_CID(SD_CID* SD_cid)
{
	uint32_t i = 0;
	SDC_Error rvalue = SD_RESPONSE_FAILURE;
	ok = false;
	uint8_t CID_Tab[16];

	/*!< Send CMD10 (CID register) */
	cmd(CMD10, 0, 0xFF);

	/*!< Wait for response in the R1 format (0x00 is no errors) */
	if (!get_response(SD_RESPONSE_NO_ERROR))
	{
		if (!get_response(0xFE))
		{
			/*!< Store CID register value on CID_Tab */
			for (i = 0; i < 16; i++)
			{
				CID_Tab[i] = read();
			}
		}
		/*!< Get CRC bytes (not really needed by us, but required by SD) */
		read();
		read();
		/*!< Set response value to success */
		ok = true;
		rvalue = SD_RESPONSE_NO_ERROR;
	}

	/*!< Send dummy byte: 8 Clock pulses of delay */
	read();

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

SDC_Error Sdc::get_CSD(SD_CSD* SD_csd)
{
	SDC_Error rvalue = SD_RESPONSE_FAILURE;
	ok = false;
	uint8_t CSD_Tab[16];

	/*!< Send CMD9 (CSD register) or CMD10(CSD register) */
	cmd(CMD9, 0, 0xFF);
	/*!< Wait for response in the R1 format (0x00 is no errors) */
	if (!get_response(SD_RESPONSE_NO_ERROR))
	{
		if (!get_response(0xFE))
		{
			for (int i = 0; i < 16; i++)
			{
				/*!< Store CSD register value on CSD_Tab */
				CSD_Tab[i] = read();
			}
		}
		/*!< Get CRC bytes (not really needed by us, but required by SD) */
		read(0xFF);
		read(0xFF);
		/*!< Set response value to success */
		ok = true;
		rvalue = SD_RESPONSE_NO_ERROR;
	}

	/*!< Send dummy byte: 8 Clock pulses of delay */
	read();

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

uint8_t Sdc::get_data_response(void)
{
	uint32_t i = 0;
	uint8_t response, rvalue;

	while (i <= 64)
	{
		/*!< Read resonse */
		response = read();
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
	while (read() == 0);

	/*!< Return response */
	return response;
}

SDC_Error Sdc::write_block(const uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize)
{
	uint32_t i = 0;
	SDC_Error rvalue = SD_RESPONSE_FAILURE;
	ok = false;


	/*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write multiple block */
	cmd(CMD24, WriteAddr, 0xFF);

	/*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
	if (!get_response(SD_RESPONSE_NO_ERROR))
	{
		/*!< Send a dummy byte */
		read();

		/*!< Send the data token to signify the start of the data */
		read(0xFE);

		/*!< Write the block data to SD : write count data by block */
		for (i = 0; i < BlockSize; i++)
		{
			/*!< Send the pointed byte */
			read(*pBuffer);
			/*!< Point to the next location where the byte read will be saved */
			pBuffer++;
		}
		/*!< Put CRC bytes (not really needed by us, but required by SD) */
		read();
		read();

		/*!< Read data response */
		if (SD_DATA_OK == get_data_response())
		{
			ok = true;
			rvalue = SD_RESPONSE_NO_ERROR;
		}
	}
	/*!< SD chip select high */
	/*!< Send dummy byte: 8 Clock pulses of delay */
	read();

	/*!< Returns the reponse */
	return rvalue;
}

SDC_Error Sdc::read_block(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize)
{
	uint32_t i = 0;
	SDC_Error rvalue = SD_RESPONSE_FAILURE;
	ok = false;

	/*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
	cmd(CMD17, ReadAddr, 0xFF);

	/*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
	if (!get_response(SD_RESPONSE_NO_ERROR))
	{
		/*!< Now look for the data token to signify the start of the data */
		if (!get_response(0xFE))
		{
			/*!< Read the SD block data : read NumByteToRead data */
			for (i = 0; i < BlockSize; i++)
			{
				/*!< Save the received data */
				*pBuffer = read();

				/*!< Point to the next location where the byte read will be saved */
				pBuffer++;
			}
			/*!< Get CRC bytes (not really needed by us, but required by SD) */
			read();
			read();
			/*!< Set response value to success */
			ok = true;
			rvalue = SD_RESPONSE_NO_ERROR;
		}
	}

	/*!< Send dummy byte: 8 Clock pulses of delay */
	read();

	/*!< Returns the reponse */
	return rvalue;
}

uint16_t Sdc::get_status(void)
{
	uint16_t Status = 0;

	/*!< Send CMD13 (SD_SEND_STATUS) to get SD status */
	cmd(ACMD13, 0, 0xFF);

	Status = read();
	Status |= (uint16_t)(read() << 8);

	/*!< Send dummy byte 0xFF */
	read();

	return Status;
}

const char *
Sdc::DRESULT2str (DRESULT r)
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

const char *
Sdc::FRESULT2str (FRESULT r)
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
}
