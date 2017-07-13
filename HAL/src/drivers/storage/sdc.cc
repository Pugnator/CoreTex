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

#ifdef SDC_DEBUG
#define DEBUG_LOG SEGGER_RTT_printf
#else
#define DEBUG_LOG(...)
#endif

void
print_readable_size (uint32_t size)
{
 const char* units[5] =
 { "B", "KB", "MB", "GB" };
 uint32_t unitIndex = 0;
 while (size >= 1024)
 {
  size /= 1024;
  ++unitIndex;
 }DEBUG_LOG(0,"%u%s\r\n",size, units[unitIndex]);
}

const char *
Sdc::command_to_str (uint8_t command)
{
 switch (command)
 {
  case 0:
   return "CMD0";
  case 1:
   return "CMD1";
  case 0x80 + 41:
   return "ACMD41";
  case 8:
   return "CMD8";
  case 9:
   return "CMD9";
  case 10:
   return "CMD10";
  case 12:
   return "CMD12";
  case 0x80 + 13:
   return "ACMD13";
  case 16:
   return "CMD16";
  case 17:
   return "CMD17";
  case 18:
   return "CMD18";
  case 0x80 + 23:
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

void
print_error (uint8_t r1)
{
 if ( r1 & SD_ERASE_RESET )
 {
  DEBUG_LOG(0,"%sSD_ERASE_RESET%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
 }
 if ( r1 & SD_ILLEGAL_COMMAND )
 {
  DEBUG_LOG(0,"%sSD_ILLEGAL_COMMAND%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
 }
 if ( r1 & SD_COM_CRC_ERROR )
 {
  DEBUG_LOG(0,"%sSD_COM_CRC_ERROR%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
 }
 if ( r1 & SD_ERASE_SEQUENCE_ERROR )
 {
  DEBUG_LOG(0,"%sSD_ERASE_SEQUENCE_ERROR%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
 }
 if ( r1 & SD_ADDRESS_ERROR )
 {
  DEBUG_LOG(0,"%sSD_ADDRESS_ERROR%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
 }
 if ( r1 & SD_PARAMETER_ERROR )
 {
  DEBUG_LOG(0,"%sSD_PARAMETER_ERROR%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
 }
}

void
Sdc::sdc_cmd (uint8_t cmd, uint32_t arg, uint8_t crc = 0)
{
 DEBUG_LOG(0,"%sCommand '%s'%s\r\n", RTT_CTRL_BG_GREEN, command_to_str(cmd), RTT_CTRL_RESET);
 if ( CMD0 != cmd )
 {
  while (0xFF != read ())
   ;
 }

 if ( cmd & 0x80 )
 {
  cmd &= 0x7F;
  DEBUG_LOG(0,"%sSend a CMD55 prior to ACMD%s\r\n", RTT_CTRL_BG_YELLOW, RTT_CTRL_RESET);
  sdc_cmd (CMD55, 0);
  if ( get_response (SD_IN_IDLE_STATE) )
  {
   DEBUG_LOG(0,"%sCMD55 failed, aborting ACMD%s\r\n", RTT_CTRL_BG_BRIGHT_RED, RTT_CTRL_RESET);
   return;
  }DEBUG_LOG(0,"%sIssue actual Application-Specific command 0x%X%s\r\n", RTT_CTRL_BG_YELLOW, cmd, RTT_CTRL_RESET);
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

 if ( CMD12 != cmd )
 {
  assert ();
 }

 uint8_t Frame[6];

 Frame[0] = (cmd | 0x40);
 Frame[1] = (uint8_t) (arg >> 24);
 Frame[2] = (uint8_t) (arg >> 16);
 Frame[3] = (uint8_t) (arg >> 8);
 Frame[4] = (uint8_t) (arg);
 Frame[5] = (crc);

 for (int i = 0; i < 6; i++)
 {
  read (Frame[i]);
 }

 if ( CMD12 == cmd )
 {
  read (0xFF);
 }
}

uint32_t
Sdc::get_card_capacity ()
{
 if ( card_capacity )
 {
  return card_capacity;
 }
 SD_CSD SD_csd = {};
 if ( SD_RESPONSE_NO_ERROR == get_CSD (&SD_csd) )
 {
  uint32_t card_capacity = (SD_csd.DeviceSize + 1);
  card_capacity *= (1 << (SD_csd.DeviceSizeMul + 2));
  card_capacity = (SD_csd.DeviceSize + 1);
  card_capacity *= (1 << (SD_csd.DeviceSizeMul + 2));
  card_block_size = 1 << (SD_csd.RdBlockLen);
  card_capacity *= card_block_size;
  DEBUG_LOG(0,"CardCapacity = %u [%u]\r\n", card_capacity, card_block_size);
  return card_capacity;
 }
 else
 {
  return 0;
 }
}

uint32_t
Sdc::get_card_block_size ()
{
 if ( card_block_size )
 {
  return card_block_size;
 }
 SD_CSD SD_csd = {};
 if ( SD_RESPONSE_NO_ERROR == get_CSD (&SD_csd) )
 {
  card_block_size = 1 << (SD_csd.RdBlockLen);
  return card_block_size;
 }
 else
 {
  return 0;
 }
}

SD_Error
Sdc::get_response (SD_Error expected_r1, SD_RESPONSE_TYPE type)
{
 DEBUG_LOG(0,"%sExpecting response 0x%X%s\r\n", RTT_CTRL_BG_BRIGHT_BLUE, expected_r1, RTT_CTRL_RESET);
 ok = false;
 word count;
 word MAX_R1_RETRY_COUNT = 10;
 for (count = 0; count < MAX_R1_RETRY_COUNT; ++count)
 {
  response = read ();
  if ( SD_DATA_OTHER_ERROR == response )
  {
   DEBUG_LOG(0,"0x%X\r\n", response);
   delay_ms (100);
   continue;
  }

  if ( response == expected_r1 )
  {
   DEBUG_LOG(0,"R1: 0x%X\r\n", response);
   break;
  }
  else
  {
   DEBUG_LOG(0,"0x%X\r\n", response);
   print_error (response);
  }
  delay_ms (100);
 }

 if ( MAX_R1_RETRY_COUNT == count )
 {
  DEBUG_LOG(0,"%sResponse timeout from SD card%s\r\n", RTT_CTRL_BG_RED, RTT_CTRL_RESET);
  print_error (response);
  return SD_RESPONSE_FAILURE;
 }

 if ( SDRT_3 == type )
 {
  DEBUG_LOG(0,"Reading trailing OCR\r\n");
  ocr = read () << 24 | read () << 16 | read () << 8 | read ();

  DEBUG_LOG(0,"%sOCR: 0x%x%s\r\n", RTT_CTRL_BG_GREEN, ocr, RTT_CTRL_RESET);
 }
 ok = true;
 return SD_RESPONSE_NO_ERROR;
}

SD_Error
Sdc::init_sd ()
{
 DEBUG_LOG(0,"%sSD v1 or MMC detected%s\r\n", RTT_CTRL_BG_GREEN, RTT_CTRL_RESET);
 uint8_t count = 5;
 do
 {
  sdc_cmd (CMD1, 0, 0xFF);
 }
 while (get_response (SD_RESPONSE_NO_ERROR) && --count);

 if ( !count )
 {
  DEBUG_LOG(0,"%sNo response from SD card%s\r\n", RTT_CTRL_BG_RED, RTT_CTRL_RESET);
  return SD_RESPONSE_FAILURE;
 }
 ok = true;
 isSDCv2 = false;
 DEBUG_LOG(0,"%sSD init OK%s\r\n", RTT_CTRL_BG_GREEN, RTT_CTRL_RESET);
 return SD_RESPONSE_NO_ERROR;
}

SD_Error
Sdc::init_sdhc ()
{
 DEBUG_LOG(0,"SDHC detected\r\n");
 //Get the remaining response after CMD8. 4 bytes
 read ();
 read ();
 if ( 0x1 != read () || 0xAA != read () ) //Actually an echo of arg = 0x1AA
 {
  DEBUG_LOG(0,"%sSD is rejected%s\r\n", RTT_CTRL_BG_RED, RTT_CTRL_RESET);
  return SD_RESPONSE_FAILURE;
 }
 else
 {
  DEBUG_LOG(0,"%sSD voltage is in range%s\r\n", RTT_CTRL_BG_GREEN, RTT_CTRL_RESET);
 }

 uint8_t count = 5;
 do
 {
  sdc_cmd (ACMD41, 1UL << 30);
 }
 while (get_response (SD_RESPONSE_NO_ERROR) && --count);

 if ( !count )
 {
  return SD_RESPONSE_FAILURE;
 }

 sdc_cmd (CMD58, 0);
 if ( get_response (SD_RESPONSE_NO_ERROR, SDRT_3) )
 {
  return SD_RESPONSE_FAILURE;
 }DEBUG_LOG(0,"%sOCR: 0x%x%s\r\n", RTT_CTRL_BG_GREEN, ocr, RTT_CTRL_RESET);
 if ( (ocr >> 30) & 1 )
 {
  DEBUG_LOG(0,"%s30th bit of OCR is set, it's SDHC%s\r\n", RTT_CTRL_BG_BRIGHT_CYAN, RTT_CTRL_RESET);
  isSDCv2 = true;
 }

 word cap = get_card_capacity ();
 return SD_RESPONSE_NO_ERROR;
}

SD_Error
Sdc::initialize (void)
{
 ok = false;
 isSDCv2 = false;
 go8bit ();
 lowspeed ();
 for (int i = 0; i <= 1000; i++)
 {
  read ();
 }
 sdc_cmd (CMD0, 0, 0x95);
 if ( get_response (SD_IN_IDLE_STATE) )
 {
  /*!< No Idle State Response: return response failue */
  DEBUG_LOG(0,"%sCMD0 failed%s\r\n", RTT_CTRL_BG_RED, RTT_CTRL_RESET);
  return SD_RESPONSE_FAILURE;
 }

 DEBUG_LOG(0,"%sCMD0 OK%s\r\n", RTT_CTRL_BG_GREEN, RTT_CTRL_RESET);

 sdc_cmd (CMD8, 0x1AA, 0x87);

 if ( get_response (SD_IN_IDLE_STATE) )
 {
  DEBUG_LOG(0,"%sCMD8 failed%s\r\n", RTT_CTRL_BG_RED, RTT_CTRL_RESET);
  return init_sd ();
 }
 else
 {
  DEBUG_LOG(0,"%sCMD8 OK%s\r\n", RTT_CTRL_BG_GREEN, RTT_CTRL_RESET);
  return init_sdhc ();
 }
}

SD_Error
Sdc::get_CID (SD_CID* SD_cid)
{
 uint32_t i = 0;
 SD_Error rvalue = SD_RESPONSE_FAILURE;
 ok = false;
 uint8_t CID_Tab[16];

 /*!< Send CMD10 (CID register) */
 sdc_cmd (CMD10, 0, 0xFF);

 /*!< Wait for response in the R1 format (0x00 is no errors) */
 if ( !get_response (SD_RESPONSE_NO_ERROR) )
 {
  if ( !get_response ((SD_Error) 0xFE) )
  {
   for (i = 0; i < 16; i++)
   {
    CID_Tab[i] = read ();
   }
  }
  read ();
  read ();
  ok = true;
  rvalue = SD_RESPONSE_NO_ERROR;
 }
 read ();
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

SD_Error
Sdc::get_CSD (SD_CSD* SD_csd)
{
 SD_Error rvalue = SD_RESPONSE_FAILURE;
 ok = false;
 uint8_t CSD_Tab[16];

 /*!< Send CMD9 (CSD register) or CMD10(CSD register) */
 sdc_cmd (CMD9, 0, 0xFF);
 /*!< Wait for response in the R1 format (0x00 is no errors) */
 if ( !get_response (SD_RESPONSE_NO_ERROR) )
 {
  if ( !get_response ((SD_Error) 0xFE) )
  {
   for (int i = 0; i < 16; i++)
   {
    /*!< Store CSD register value on CSD_Tab */
    CSD_Tab[i] = read ();
   }
  }
  /*!< Get CRC bytes (not really needed by us, but required by SD) */
  read (0xFF);
  read (0xFF);
  /*!< Set response value to success */
  ok = true;
  rvalue = SD_RESPONSE_NO_ERROR;
 }

 /*!< Send dummy byte: 8 Clock pulses of delay */
 read ();

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

uint8_t
Sdc::get_data_response (void)
{
 uint32_t i = 0;
 uint8_t response, rvalue;

 while (i <= 64)
 {
  /*!< Read resonse */
  response = read ();
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
  if ( rvalue == SD_DATA_OK )
   break;
  /*!< Increment loop counter */
  i++;
 }

 /*!< Wait null data */
 while (read () == 0)
  ;

 /*!< Return response */
 return response;
}

SD_Error
Sdc::write_block (const uint8_t* pBuffer, uint32_t WriteAddr,
                  uint16_t BlockSize)
{
 LEDON;
 uint32_t i = 0;
 SD_Error rvalue = SD_RESPONSE_FAILURE;
 ok = false;

 /*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write multiple block */
 sdc_cmd (CMD24, WriteAddr, 0xFF);

 /*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
 if ( !get_response (SD_RESPONSE_NO_ERROR) )
 {
  /*!< Send a dummy byte */
  read ();

  /*!< Send the data token to signify the start of the data */
  read (0xFE);

  /*!< Write the block data to SD : write count data by block */
  for (i = 0; i < BlockSize; i++)
  {
   /*!< Send the pointed byte */
   read (*pBuffer);
   /*!< Point to the next location where the byte read will be saved */
   pBuffer++;
  }
  /*!< Put CRC bytes (not really needed by us, but required by SD) */
  read ();
  read ();

  /*!< Read data response */
  if ( SD_DATA_OK == get_data_response () )
  {
   ok = true;
   rvalue = SD_RESPONSE_NO_ERROR;
  }
 }
 /*!< SD chip select high */
 /*!< Send dummy byte: 8 Clock pulses of delay */
 read ();

 /*!< Returns the reponse */
 LEDOFF;
 return rvalue;
}

SD_Error
Sdc::read_block (uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize)
{
 LEDON;
 DEBUG_LOG(0,"Sdc::read_block, Address=%u, BlockSize=%u\r\n", ReadAddr, BlockSize);
 uint32_t i = 0;
 SD_Error rvalue = SD_RESPONSE_FAILURE;
 ok = false;

 /*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
 sdc_cmd (CMD17, ReadAddr, 0xFF);

 /*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
 if ( !get_response (SD_RESPONSE_NO_ERROR) )
 {
  /*!< Now look for the data token to signify the start of the data */
  if ( !get_response ((SD_Error) 0xFE) )
  {
   /*!< Read the SD block data : read NumByteToRead data */
   for (i = 0; i < BlockSize; i++)
   {
    /*!< Save the received data */
    *pBuffer = read ();
    /*!< Point to the next location where the byte read will be saved */
    pBuffer++;
   }
   /*!< Get CRC bytes (not really needed by us, but required by SD) */
   read ();
   read ();
   /*!< Set response value to success */
   ok = true;
   rvalue = SD_RESPONSE_NO_ERROR;
  }
 }

 /*!< Send dummy byte: 8 Clock pulses of delay */
 read ();

 /*!< Returns the reponse */
 LEDOFF;
 return rvalue;
}

uint16_t
Sdc::get_status (void)
{
 uint16_t Status = 0;

 /*!< Send CMD13 (SD_SEND_STATUS) to get SD status */
 sdc_cmd (ACMD13, 0, 0xFF);

 Status = read ();
 Status |= (uint16_t) (read () << 8);

 /*!< Send dummy byte 0xFF */
 read ();

 return Status;
}
