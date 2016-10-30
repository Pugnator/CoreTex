#pragma once

#include <drivers/storage/ff.hpp>
#include <core/spi.hpp>

namespace SDC
{
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
	} SDC_Error;

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


	class Sdc: public SPI::Spi
	{
	public:
		Sdc(char channel)
	: SPI::Spi(channel)
		{
			card_capacity = 0;
			card_block_size = 0;
			ok = false;
			isSDCv2 = false;
		}
	public:
		uint8_t detect(void);
		SDC_Error initialize(void);
		SDC_Error read_block(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize);
		SDC_Error write_block(const uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize);
		SDC_Error get_response(uint8_t Response);
		SDC_Error get_CID(SD_CID* SD_cid);
		SDC_Error get_CSD(SD_CSD* SD_csd);

		uint8_t get_data_response(void);
		uint16_t sdc_cmd(uint8_t Cmd, uint32_t Arg, uint8_t Crc);
		uint16_t get_status(void);
		uint32_t get_card_block_size();
		uint32_t get_card_capacity();

		const char *command_to_str (uint8_t command);
		bool ok;
	protected:
		SDC_Error init_sd(void);
		SDC_Error init_sdhc(void);
	protected:
		bool isSDCv2;
		uint32_t card_capacity;
		uint32_t card_block_size;
	};
}
