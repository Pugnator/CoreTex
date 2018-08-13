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

#include <drivers/mfrc522.hpp>
#include <common.hpp>
#include<log.hpp>

namespace MIFARE
{
 RFID::RFID (char ch)
	 : Spi (ch)
 {
	PIN_OUT_PP(MFC522RST_PIN);
	PIN_LOW(MFC522RST_PIN);
	PIN_HI(MFC522RST_PIN);
	reset();
	current_tag_type = MIFARE_ULTRALIGHT;
 }

 uint8_t RFID::version ()
 {
	return regr (VERSIONREG);
 }

 uint8_t RFID::regr (MIF_REG reg)
 {
	read (((reg << 1) & 0x7E) | 0x80);
	return read (0x00);
 }

 void RFID::regw (MIF_REG reg, uint8_t data)
 {
	read ((reg << 1) & 0x7E);
	read (data);
 }

 bool RFID::check ()
 {
	uint8_t ver = version ();
	return 0x92 == ver || 0x91 == ver;
 }

 MI_ERROR RFID::is_tag_available ()
 {
	regw(BITFRAMINGREG, 0x07);
	payload.clear();
	payload.push_back(MF1_REQIDL);
	uint16_t bnum = 0;
	MI_ERROR status = tag_command(TRANSCEIVE_CMD, &bnum);
	PrintF("Usable bits: %u\n", bnum);

	if ((status != MI_OK) || (bnum != 0x10))
	{
	  status = MI_ERR;
	}
	return status;
 }

 MI_ERROR RFID::tag_command (MIF_CMD cmd, uint16_t *bits_num)
 {
	MI_ERROR status = MI_ERR;
	uint8_t irqEn = 0x00;
	uint8_t waitIRq = 0x00;

	switch (cmd)
	{
	 case AUTHENT_CMD:
		irqEn = 0x12;
		waitIRq = 0x10;
		break;
	 case TRANSCEIVE_CMD:
		irqEn = 0x77;
		waitIRq = 0x30;
		break;
	 default:
		break;
	}

	regw (COMMIENREG, irqEn | 0x80);    // interrupt request
	reg_unmask (COMMIENREG, 0x80);           // Clear all interrupt requests bits.
	reg_mask (FIFOLEVELREG, 0x80); // FlushBuffer=1, FIFO initialization.

	regw (COMMANDREG, IDLE_CMD); // No action, cancel the current command.

	for (MI_DATA_ITER it = payload.begin (); it != payload.end (); ++it)
	{
	 PrintF("FIFO <= 0x%X\n", *it);
	 regw (FIFODATAREG, *it);
	}

	regw (COMMANDREG, cmd);
	if (cmd == TRANSCEIVE_CMD)
	{
	 reg_mask (BITFRAMINGREG, 0x80); // StartSend=1, transmission of data starts
	}

	uint8_t retries = 25; // Max wait time is 25ms.
	uint8_t n = 0;
	do
	{
	 PrintF("Wait..%u\n", retries);
	 delay_ms (1);
	 // CommIRqReg[7..0]
	 // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
	 n = regr (COMMIRQREG);
	 retries--;
	}
	while ((retries != 0) && !(n & 0x01) && !(n & waitIRq));

	reg_unmask (BITFRAMINGREG, 0x80); // StartSend=0

	if (!retries || (regr (ERRORREG) & 0x1D)) //timed out or errored
	{
	 Print("timed out\n");
	 return MI_ERR;
	}

	status = MI_OK;
	if (n & irqEn & 0x01)
	{
	 Print("No tags error\n");
	 status = MI_NOTAGERR;
	}

	if (cmd == TRANSCEIVE_CMD)
	{
	 n = regr (FIFOLEVELREG);
	 uint8_t lastBits = regr (CONTROLREG) & 0x07;
	 PrintF("Last bit: 0x%X [n: 0x%X]\n", lastBits, n);
	 if (lastBits)
	 {
		*bits_num = (n - 1) * 8 + lastBits;
	 }
	 else
	 {
		*bits_num = n * 8;
	 }

	 if (n == 0)
	 {
		n = 1;
	 }

	 if (n > 16)
	 {
		n = 16;
	 }

	 // Reading the recieved data from FIFO.
	 for (word i = 0; i < n; i++)
	 {
		result.push_back(regr(FIFODATAREG));
	 }
	}
	return status;
 }

 void RFID::reg_mask (MIF_REG reg, uint8_t mask)
 {
	uint8_t res = regr (reg);
	regw (reg, res | mask);
 }

 void RFID::reg_unmask (MIF_REG reg, uint8_t mask)
 {
	uint8_t res = regr (reg);
	regw (reg, res & (~mask));
 }

 void RFID::reset()
 {
	regw(COMMANDREG, SOFTRESET_CMD);
			// The datasheet does not mention how long the SoftRest command takes to complete.
			// But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg)
			// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74us. Let us be generous: 50ms.
			delay_ms(50);
			while ((regr(COMMANDREG) & (1<<4)) != 0)
			{
				// PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
			}
 }

 bool RFID::selftest()
 {
	reset();
	regw(FIFOLEVELREG, 0x80);
	return true;
 }

} //namespace MIFARE
