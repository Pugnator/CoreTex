#pragma once

#include <drivers/storage/ff.hpp>
#include <core/spi.hpp>
#include <string.h>

typedef enum
{
 SD_RESPONSE_NO_ERROR = (0x00),
 SD_IN_IDLE_STATE = (0x01),
 SD_ERASE_RESET = (0x02),
 SD_ILLEGAL_COMMAND = (0x04),
 SD_COM_CRC_ERROR = (0x08),
 SD_ERASE_SEQUENCE_ERROR = (0x10),
 SD_ADDRESS_ERROR = (0x20),
 SD_PARAMETER_ERROR = (0x40),
 SD_RESPONSE_FAILURE = (0xFF),

 SD_DATA_OK = (0x05),
 SD_DATA_CRC_ERROR = (0x0B),
 SD_DATA_WRITE_ERROR = (0x0D),
 SD_DATA_OTHER_ERROR = (0xFF)
} SD_Error;

typedef struct
{
 uint32_t DeviceSize; /*!< Device Size */
 uint16_t CardComdClasses; /*!< Card command classes */
 uint8_t CSDStruct; /*!< CSD structure */
 uint8_t SysSpecVersion; /*!< System specification version */
 uint8_t Reserved1; /*!< Reserved */
 uint8_t TAAC; /*!< Data read access-time 1 */
 uint8_t NSAC; /*!< Data read access-time 2 in CLK cycles */
 uint8_t MaxBusClkFrec; /*!< Max. bus clock frequency */
 uint8_t RdBlockLen; /*!< Max. read data block length */
 uint8_t PartBlockRead; /*!< Partial blocks for read allowed */
 uint8_t WrBlockMisalign; /*!< Write block misalignment */
 uint8_t RdBlockMisalign; /*!< Read block misalignment */
 uint8_t DSRImpl; /*!< DSR implemented */
 uint8_t Reserved2; /*!< Reserved */
 uint8_t MaxRdCurrentVDDMin; /*!< Max. read current @ VDD min */
 uint8_t MaxRdCurrentVDDMax; /*!< Max. read current @ VDD max */
 uint8_t MaxWrCurrentVDDMin; /*!< Max. write current @ VDD min */
 uint8_t MaxWrCurrentVDDMax; /*!< Max. write current @ VDD max */
 uint8_t DeviceSizeMul; /*!< Device size multiplier */
 uint8_t EraseGrSize; /*!< Erase group size */
 uint8_t EraseGrMul; /*!< Erase group size multiplier */
 uint8_t WrProtectGrSize; /*!< Write protect group size */
 uint8_t WrProtectGrEnable; /*!< Write protect group enable */
 uint8_t ManDeflECC; /*!< Manufacturer default ECC */
 uint8_t WrSpeedFact; /*!< Write speed factor */
 uint8_t MaxWrBlockLen; /*!< Max. write data block length */
 uint8_t WriteBlockPaPartial; /*!< Partial blocks for write allowed */
 uint8_t Reserved3; /*!< Reserded */
 uint8_t ContentProtectAppli; /*!< Content protection application */
 uint8_t FileFormatGrouop; /*!< File format group */
 uint8_t CopyFlag; /*!< Copy flag (OTP) */
 uint8_t PermWrProtect; /*!< Permanent write protection */
 uint8_t TempWrProtect; /*!< Temporary write protection */
 uint8_t FileFormat; /*!< File Format */
 uint8_t ECC; /*!< ECC code */
 uint8_t CSD_CRC; /*!< CSD CRC */
 uint8_t Reserved4; /*!< always 1*/
} SD_CSD;

typedef struct
{
 uint32_t ProdSN; /*!< Product Serial Number */
 uint32_t ProdName1; /*!< Product Name part1 */
 uint16_t ManufactDate; /*!< Manufacturing Date */
 uint16_t OEM_AppliID; /*!< OEM/Application ID */
 uint8_t ManufacturerID; /*!< ManufacturerID */
 uint8_t ProdName2; /*!< Product Name part2*/
 uint8_t ProdRev; /*!< Product Revision */
 uint8_t Reserved1; /*!< Reserved1 */
 uint8_t CID_CRC; /*!< CID CRC */
 uint8_t Reserved2; /*!< always 1 */
} SD_CID;

/* MMC/SD command */
typedef enum
{
 CMD0 = 0, /* GO_IDLE_STATE */
 CMD1 = 1, /* SEND_OP_COND (MMC) */
 ACMD41 = 0x80 + 41, /* SEND_OP_COND (SDC) */
 CMD8 = 8, /* SEND_IF_COND */
 CMD9 = 9, /* SEND_CSD */
 CMD10 = 10, /* SEND_CID */
 CMD12 = 12, /* STOP_TRANSMISSION */
 ACMD13 = 0x80 + 13, /* SD_STATUS (SDC) */
 CMD16 = 16, /* SET_BLOCKLEN */
 CMD17 = 17, /* READ_SINGLE_BLOCK */
 CMD18 = 18, /* READ_MULTIPLE_BLOCK */
 CMD23 = 23, /* SET_BLOCK_COUNT (MMC) */
 ACMD23 = 0x80 + 23, /* SET_WR_BLK_ERASE_COUNT (SDC) */
 CMD24 = 24, /* WRITE_BLOCK */
 CMD25 = 25, /* WRITE_MULTIPLE_BLOCK */
 CMD32 = 32, /* ERASE_ER_BLK_START */
 CMD33 = 33, /* ERASE_ER_BLK_END */
 CMD38 = 38, /* ERASE */
 CMD55 = 55, /* APP_CMD */
 CMD58 = 58, /* READ_OCR */
} SD_COMMAND;

typedef enum
{
 SDRT_UNSPECIFIED = 0,
 SDRT_NONE = 1,
 SDRT_1 = 2,
 SDRT_1B = 3,
 SDRT_2 = 4,
 SDRT_3 = 5,
 SDRT_4 = 6,
 SDRT_5 = 7,
 SDRT_5B = 8,
 SDRT_6 = 9
} SD_RESPONSE_TYPE;

class Sdc : public Spi
{
public:
 Sdc (char channel) :
   Spi (channel)
 {
  response = 0;
  ocr = 0;
  card_capacity = 0;
  card_block_size = 0;
  ok = false;
  isSDCv2 = false;
 }
public:
 uint8_t
 detect (void);
 SD_Error
 initialize (void);
 SD_Error
 read_block (uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize);
 SD_Error
 write_block (const uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize);
 SD_Error
 get_response (SD_Error response, SD_RESPONSE_TYPE type = SDRT_1);
 SD_Error
 get_CID (SD_CID* SD_cid);
 SD_Error
 get_CSD (SD_CSD* SD_csd);

 uint8_t
 get_data_response (void);
 void
 sdc_cmd (uint8_t cmd, uint32_t arg, uint8_t crc);
 uint16_t
 get_status (void);
 uint32_t
 get_card_block_size ();
 uint32_t
 get_card_capacity ();

 const char *
 command_to_str (uint8_t command);
 bool ok;
protected:
 void
 dummy_read (void);
 SD_Error
 init_sd (void);
 SD_Error
 init_sdhc (void);
protected:
 uint8_t response;
 word ocr;
 bool isSDCv2;
 uint32_t card_capacity;
 uint32_t card_block_size;
};
