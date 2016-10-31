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
	CMD0	=    0,            /* GO_IDLE_STATE */
	CMD1	=    1,            /* SEND_OP_COND (MMC) */
	ACMD41	= 	 0x80+41,      /* SEND_OP_COND (SDC) */
	CMD8	=    8,            /* SEND_IF_COND */
	CMD9	=    9,            /* SEND_CSD */
	CMD10	=   10,            /* SEND_CID */
	CMD12	=   12,            /* STOP_TRANSMISSION */
	ACMD13	=	0x80+13,       /* SD_STATUS (SDC) */
	CMD16	=   16,            /* SET_BLOCKLEN */
	CMD17	=   17,            /* READ_SINGLE_BLOCK */
	CMD18	=   18,            /* READ_MULTIPLE_BLOCK */
	CMD23	=   23,            /* SET_BLOCK_COUNT (MMC) */
	ACMD23	= 	0x80+23,       /* SET_WR_BLK_ERASE_COUNT (SDC) */
	CMD24	=   24,            /* WRITE_BLOCK */
	CMD25	=   25,            /* WRITE_MULTIPLE_BLOCK */
	CMD32	=   32,            /* ERASE_ER_BLK_START */
	CMD33	=   33,            /* ERASE_ER_BLK_END */
	CMD38	=   38,            /* ERASE */
	CMD55	=   55,            /* APP_CMD */
	CMD58	=   58,            /* READ_OCR */
}SDCCMD;

void print_readable_size(uint32_t size)
{
	const char* units[5] = { "B", "KB", "MB", "GB"};
	uint32_t unitIndex = 0;
	while (size >= 1024)
	{
		size /= 1024;
		++unitIndex;
	}
	LOGPRINT("%u%s\r\n",size, units[unitIndex]);
}

const char *Sdc::command_to_str (uint8_t command)
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

void print_error(uint8_t r1)
{
	if(0xFF == r1)
	{
		return;
	}
	if( r1 & SD_IN_IDLE_STATE)
	{
		LOGPRINT("SD_IN_IDLE_STATE\r\n");
	}
	if( r1 & SD_ERASE_RESET)
	{
		LOGPRINT("SD_ERASE_RESET\r\n");
	}
	if( r1 & SD_ILLEGAL_COMMAND)
	{
		LOGPRINT("SD_ILLEGAL_COMMAND\r\n");
	}
	if( r1 & SD_COM_CRC_ERROR)
	{
		LOGPRINT("SD_COM_CRC_ERROR\r\n");
	}
	if( r1 & SD_ERASE_SEQUENCE_ERROR)
	{
		LOGPRINT("SD_ERASE_SEQUENCE_ERROR\r\n");
	}
	if( r1 & SD_ADDRESS_ERROR)
	{
		LOGPRINT("SD_ADDRESS_ERROR\r\n");
	}
	if( r1 & SD_PARAMETER_ERROR)
	{
		LOGPRINT("SD_PARAMETER_ERROR\r\n");
	}
	if( r1 & SD_RESPONSE_FAILURE)
	{
		LOGPRINT("SD_RESPONSE_FAILURE\r\n");
	}
}

uint16_t Sdc::sdc_cmd(uint8_t cmd, uint32_t arg, uint8_t crc = 0)
{
	LOGPRINT("sdc_cmd: %s\r\n", command_to_str(cmd));
	if(CMD0 != cmd)
	{
		do
		{

		}while(0xFF != read());
	}
	uint16_t res = 0;
	if (cmd & 0x80)
	{	/* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		res = sdc_cmd(CMD55, 0);
		if (res > 1)
		{
			return res;
		}
	}
	switch (cmd)
	{
	case CMD0:
		crc = 0x95;
		break;
	case CMD8:
		crc = 0x87;
		break;
	}

	if(CMD12 != cmd)
	{
		assert();
	}

	uint8_t Frame[6];

	Frame[0] = (cmd | 0x40); /*!< Construct byte 1 */
	Frame[1] = (uint8_t)(arg >> 24); /*!< Construct byte 2 */
	Frame[2] = (uint8_t)(arg >> 16); /*!< Construct byte 3 */
	Frame[3] = (uint8_t)(arg >> 8); /*!< Construct byte 4 */
	Frame[4] = (uint8_t)(arg); /*!< Construct byte 5 */
	Frame[5] = (crc); /*!< Construct CRC: byte 6 */

	for (int i = 0; i < 6; i++)
	{
		res = read(Frame[i]); /*!< Send the Cmd bytes */
	}

	if (CMD12 == cmd)
	{
		read(0xFF);
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
		LOGPRINT("CardCapacity = ");
		print_readable_size(card_capacity);
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
	LOGPRINT("get_response: expecting 0x%X\r\n", Response);
	ok = false;
	uint32_t Count = 10;
	/*!< Check if response is got or a timeout is happen */
	uint16_t r1 = 0;
	do
	{
		r1 = read();
		if(0xFF == r1)
		{
			continue;
		}
		LOGPRINT("R: 0x%X\r\n", r1);
		break;
	}while (r1 != Response && --Count);

	if (Count == 0 || r1 != Response)
	{
		/*!< After time out */
		print_error(r1);
		return SD_RESPONSE_FAILURE;
	}
	else
	{
		/*!< Right response got */
		ok = true;
		return SD_RESPONSE_NO_ERROR;
	}
}

SDC_Error Sdc::init_sd()
{
	LOGPRINT("SD v1 or MMC detected\r\n");
	uint8_t count = 100;
	do
	{
		read();
		sdc_cmd(CMD1, 0, 0xFF);
	}
	while (get_response(SD_RESPONSE_NO_ERROR) && --count);

	if(!count)
	{
		LOGPRINT("No response from SD card\r\n");
		return SD_RESPONSE_FAILURE;
	}
	ok = true;
	isSDCv2 = false;
	LOGPRINT("SD init OK\r\n");
	return SD_RESPONSE_NO_ERROR;
}

SDC_Error Sdc::init_sdhc()
{
	LOGPRINT("SDHC detected\r\n");
	//Get the remaining response after CMD8. 4 bytes
	read();
	read();
	if(0x1 != read() || 0xAA != read()) //Actually an echo of arg = 0x1AA
	{
		LOGPRINT("SD is rejected\r\n");
		return SD_RESPONSE_FAILURE;
	}
	LOGPRINT("SD v2\r\n");
	uint8_t count = 100;
	do
	{
		sdc_cmd(ACMD41, 1UL << 30);
		delay_ms(500);
	}
	while(get_response(SD_IN_IDLE_STATE) && --count);
	if(!count)
	{
		return SD_RESPONSE_FAILURE;
	}

	sdc_cmd(CMD58, 0);
	if(get_response(SD_IN_IDLE_STATE))
	{
		return SD_RESPONSE_FAILURE;
	}
	LOGPRINT("SD - v2, ID: 0x%X\r\n", read() & 0x40);
	read();
	read();
	read();
	sdc_cmd(CMD16, 666);
	if(get_response(SD_IN_IDLE_STATE))
	{
		LOGPRINT("Failed to set block size 512\r\n");
		return SD_RESPONSE_FAILURE;
	}
	LOGPRINT("Block size was set to 512\r\n");
	isSDCv2 = true;
	return SD_RESPONSE_NO_ERROR;
}

SDC_Error Sdc::initialize(void)
{
	ok = false;
	go8bit();
	highspeed();
	for (int i = 0; i <=100; i++)
	{
		/*!< Send dummy byte 0xFF */
		read();
	}
	/*!< Send CMD0 (SD_CMD_GO_IDLE_STATE) to put SD in SPI mode */

	sdc_cmd(CMD0, 0, 0x95);

	/*!< Wait for In Idle State Response (R1 Format) equal to 0x01 */
	if (get_response(SD_IN_IDLE_STATE))
	{
		/*!< No Idle State Response: return response failue */
		LOGPRINT("CMD0 failed\r\n");
		return SD_RESPONSE_FAILURE;
	}

	LOGPRINT("CMD0 OK\r\n");

	sdc_cmd(CMD8, 0x1AA, 0x87);

	if(get_response(SD_IN_IDLE_STATE))
	{
		LOGPRINT("CMD8 failed\r\n");
		return init_sd();
	}
	else
	{
		LOGPRINT("CMD8 OK\r\n");
		return init_sdhc();
	}
}

SDC_Error Sdc::get_CID(SD_CID* SD_cid)
{
	uint32_t i = 0;
	SDC_Error rvalue = SD_RESPONSE_FAILURE;
	ok = false;
	uint8_t CID_Tab[16];

	/*!< Send CMD10 (CID register) */
	sdc_cmd(CMD10, 0, 0xFF);

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
	sdc_cmd(CMD9, 0, 0xFF);
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
	sdc_cmd(CMD24, WriteAddr, 0xFF);

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
	LOGPRINT("Sdc::read_block, Address=%u, BlockSize=%u\r\n", ReadAddr, BlockSize);
	uint32_t i = 0;
	SDC_Error rvalue = SD_RESPONSE_FAILURE;
	ok = false;

	/*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
	sdc_cmd(CMD17, ReadAddr, 0xFF);

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
	sdc_cmd(ACMD13, 0, 0xFF);

	Status = read();
	Status |= (uint16_t)(read() << 8);

	/*!< Send dummy byte 0xFF */
	read();

	return Status;
}
}
