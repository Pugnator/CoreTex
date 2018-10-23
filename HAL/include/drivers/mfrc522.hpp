#pragma once

#include <core/spi.hpp>
#include <list>

namespace MIFARE
{

 typedef enum
 {
	VER_UNKNOWN = 0,
	VER_FM17522 = 0x88,
	VER_0_0 = 0x90,
	VER_1_0 = 0x91,
	VER_2_0 = 0x92
 } MIF_VER;

 typedef enum
 {
	IDLE_CMD = 0x00, // NO action; Cancel the current command
	MEM_CMD = 0x01, // Store 25 byte into the internal buffer.
	GENID_CMD = 0x02, // Generates a 10 byte random ID number.
	CALCCRC_CMD = 0x03, // CRC Calculate or selftest.
	TRANSMIT_CMD = 0x04, // Transmit data
	NOCMDCH_CMD = 0x07, // No command change.
	RECEIVE_CMD = 0x08, // Receive Data
	TRANSCEIVE_CMD = 0x0C, // Transmit and receive data,
	AUTHENT_CMD = 0x0E, // Authentication Key
	SOFTRESET_CMD = 0x0F // Reset
 } MIF_CMD;

 typedef enum
 {
	MF1_REQIDL = 0x26,      // find the antenna area does not enter hibernation
	MF1_REQALL = 0x52,      // find all the tags antenna area
	MF1_ANTICOLL = 0x93,      // anti-collision
	MF1_SELECTTAG = 0x93,      // election tag
	MF1_AUTHENT1A = 0x60,      // authentication key A
	MF1_AUTHENT1B = 0x61,      // authentication key B
	MF1_READ = 0x30,      // Read Block
	MF1_WRITE = 0xA0,      // write block
	MF1_DECREMENT = 0xC0,      // debit
	MF1_INCREMENT = 0xC1,      // recharge
	MF1_RESTORE = 0xC2,      // transfer block data to the buffer
	MF1_TRANSFER = 0xB0,      // save the data in the buffer
	MF1_HALT = 0x50      // Sleep
 } TAG_CMD;

 typedef enum
 {
	MI_OK = 0, MI_NOTAGERR, MI_ERR
 } MI_ERROR;

 typedef enum
 {
	RESERVED00,
	COMMANDREG,
	COMMIENREG,
	DIVIENREG,
	COMMIRQREG,
	DIVIRQREG,
	ERRORREG,
	STATUS1REG,
	STATUS2REG,
	FIFODATAREG,
	FIFOLEVELREG,
	WATERLEVELREG,
	CONTROLREG,
	BITFRAMINGREG,
	COLLREG,
	RESERVED01,
	RESERVED10,
	MODEREG,
	TXMODEREG,
	RXMODEREG,
	TXCONTROLREG,
	TXAUTOREG,
	TXSELREG,
	RXSELREG,
	RXTHRESHOLDREG,
	DEMODREG,
	RESERVED11,
	RESERVED12,
	MIFAREREG,
	RESERVED13,
	RESERVED14,
	SERIALSPEEDREG,
	RESERVED20,
	CRCRESULTREGM,
	CRCRESULTREGL,
	RESERVED21,
	MODWIDTHREG,
	RESERVED22,
	RFCFGREG,
	GSNREG,
	CWGSPREG,
	MODGSPREG,
	TMODEREG,
	TPRESCALERREG,
	TRELOADREGH,
	TRELOADREGL,
	TCOUNTERVALUEREGH,
	TCOUNTERVALUEREGL,
	RESERVED30,
	TESTSEL1REG,
	TESTSEL2REG,
	TESTPINENREG,
	TESTPINVALUEREG,
	TESTBUSREG,
	AUTOTESTREG,
	VERSIONREG,
	ANALOGTESTREG,
	TESTDAC1REG,
	TESTDAC2REG,
	TESTADCREG,
	RESERVED31,
	RESERVED32,
	RESERVED33,
	RESERVED34,
 } MIF_REG;

 typedef enum
 {
	MIFARE_ULTRALIGHT = 0x4400,
	MIFARE_ONE_S50 = 0x0400,
	MIFARE_ONE_S70 = 0x0200,
	MIFARE_PRO = 0x0800,
	MIFARE_DESFIRE = 0x4403
 } MIFARE_TYPE;

 typedef std::list<uint8_t> MI_DATA;
 typedef std::list<uint8_t>::iterator MI_DATA_ITER;

#define MFC522RST_PIN A,3,SPEED_50MHz
#define SPEED 115200

 class RFID : public Spi
 {
 public:
	RFID (char ch);
	~RFID ()
	{
	}
	;

 public:
	void reset ();
	MIF_VER version ();
	bool check ();
	bool selftest ();
	MI_ERROR is_tag_available ();
	MI_ERROR tag_command (MIF_CMD cmd, uint16_t *bits_num);

 private:
	void reg_mask (MIF_REG reg, uint8_t mask);
	void reg_unmask (MIF_REG reg, uint8_t mask);
	uint8_t regr (MIF_REG reg);
	void regw (MIF_REG reg, uint8_t data);
	MIFARE_TYPE current_tag_type;
	MI_DATA result;
	MI_DATA payload;
 };
} //namespace MIFARE
