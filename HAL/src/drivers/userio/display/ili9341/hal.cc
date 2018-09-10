/**
 *
 * @file   hal.c
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @copyright Copyright 2015 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the GPL 2 license.
 * @brief HAL for ili9341
 *
 */

#include <drivers/ili9341.hpp>
#include <common.hpp>
#include <log.hpp>

namespace GLCD
{

 /**********************************************************************************************//**
  * \fn	uint8_t send (TFT_MODE mode, uint8_t data)
  *
  * \brief	Send the data to the display
  *
  * \param [in] mode Command or data is to be sent
  * \param [in]	data	If non-null, the name.
  **************************************************************************************************/

 uint8_t TFT::send (TFT_MODE mode, uint8_t data)
 {
	if (CMD == mode)
	{
	 PIN_LOW(DC);
	}
	else
	{
	 PIN_HI(DC);
	}
	return read (data);
 }
 
 void TFT::set_color (uint16_t color)
 {
	current_color = color;
 }

 void TFT::set_cursor (uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 {
	send (CMD, ILI9341_COLUMN_ADDR);
	send (DATA, x1 >> 8);
	send (DATA, x1 & 0xFF);
	send (DATA, x2 >> 8);
	send (DATA, x2 & 0xFF);

	send (CMD, ILI9341_PAGE_ADDR);
	send (DATA, y1 >> 8);
	send (DATA, y1 & 0xFF);
	send (DATA, y2 >> 8);
	send (DATA, y2 & 0xFF);
 }

 void TFT::set_pixel (uint16_t x, uint16_t y)
 {
	set_cursor (x, y, x, y);
	send (CMD, ILI9341_GRAM);
	send (DATA, current_color >> 8);
	send (DATA, current_color & 0xFF);
 }

 void TFT::configure ()
 {
	PIN_LOW(RESET);
	delay_ms (500);
	PIN_HI(RESET);

	send (CMD, ILI9341_RESET);
	delay_ms (100);

	send (CMD, ILI9341_POWERA);
	send (DATA, 0x39);
	send (DATA, 0x2C);
	send (DATA, 0x00);
	send (DATA, 0x34);
	send (DATA, 0x02);
	send (CMD, ILI9341_POWERB);
	send (DATA, 0x00);
	send (DATA, 0xC1);
	send (DATA, 0x30);
	send (CMD, ILI9341_DTCA);
	send (DATA, 0x85);
	send (DATA, 0x00);
	send (DATA, 0x78);
	send (CMD, ILI9341_DTCB);
	send (DATA, 0x00);
	send (DATA, 0x00);
	send (CMD, ILI9341_POWER_SEQ);
	send (DATA, 0x64);
	send (DATA, 0x03);
	send (DATA, 0x12);
	send (DATA, 0x81);
	send (CMD, ILI9341_PRC);
	send (DATA, 0x20);
	send (CMD, ILI9341_POWER1);
	send (DATA, 0x23);
	send (CMD, ILI9341_POWER2);
	send (DATA, 0x10);
	send (CMD, ILI9341_VCOM1);
	send (DATA, 0x3E);
	send (DATA, 0x28);
	send (CMD, ILI9341_VCOM2);
	send (DATA, 0x86);
	send (CMD, ILI9341_MAC);
	send (DATA, 0x48);
	send (CMD, ILI9341_PIXEL_FORMAT);
	send (DATA, 0x55);
	send (CMD, ILI9341_FRC);
	send (DATA, 0x00);
	send (DATA, 0x18);
	send (CMD, ILI9341_DFC);
	send (DATA, 0x08);
	send (DATA, 0x82);
	send (DATA, 0x27);
	send (CMD, ILI9341_3GAMMA_EN);
	send (DATA, 0x00);
	send (CMD, ILI9341_COLUMN_ADDR);
	send (DATA, 0x00);
	send (DATA, 0x00);
	send (DATA, 0x00);
	send (DATA, 0xEF);
	send (CMD, ILI9341_PAGE_ADDR);
	send (DATA, 0x00);
	send (DATA, 0x00);
	send (DATA, 0x01);
	send (DATA, 0x3F);
	send (CMD, ILI9341_GAMMA);
	send (DATA, 0x01);
	send (CMD, ILI9341_PGAMMA);
	send (DATA, 0x0F);
	send (DATA, 0x31);
	send (DATA, 0x2B);
	send (DATA, 0x0C);
	send (DATA, 0x0E);
	send (DATA, 0x08);
	send (DATA, 0x4E);
	send (DATA, 0xF1);
	send (DATA, 0x37);
	send (DATA, 0x07);
	send (DATA, 0x10);
	send (DATA, 0x03);
	send (DATA, 0x0E);
	send (DATA, 0x09);
	send (DATA, 0x00);
	send (CMD, ILI9341_NGAMMA);
	send (DATA, 0x00);
	send (DATA, 0x0E);
	send (DATA, 0x14);
	send (DATA, 0x03);
	send (DATA, 0x11);
	send (DATA, 0x07);
	send (DATA, 0x31);
	send (DATA, 0xC1);
	send (DATA, 0x48);
	send (DATA, 0x08);
	send (DATA, 0x0F);
	send (DATA, 0x0C);
	send (DATA, 0x31);
	send (DATA, 0x36);
	send (DATA, 0x0F);
	send (CMD, ILI9341_SLEEP_OUT);

	delay_ms (100);
	send (CMD, ILI9341_DISPLAY_ON);
	send (CMD, ILI9341_GRAM);
 }

 uint32_t TFT::reg_read (uint8_t command, uint8_t parameter)
 {
	send (CMD, 0xd9);
	send (DATA, 0x10 + parameter);
	send (CMD, command);
	return send (DATA, 0);
 }

 bool TFT::check ()
 {
	return 0 == reg_read (0xd3, 1) && 0x93 == reg_read (0xd3, 2) && 0x41 == reg_read (0xd3, 3);
 }

 void TFT::sleep ()
 {
	send (CMD, 0x10);
 }
}
