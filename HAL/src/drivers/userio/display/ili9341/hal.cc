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

 void TFT::clear_display()
 {
	fill_display(0);
 }

 void TFT::fill_display(uint16_t color)
 {
	set_color(color);
	set_frame(0, 239, 0, 319);
	for (uint16_t i = 0; i < 38400; i++)
	{
	 send(DATA, color);
	}
 }

 /**********************************************************************************************//**
  * \fn	uint8_t send (TFT_MODE mode, uint8_t data)
  *
  * \brief	Send the data to the display
  *
  * \param [in] mode Command or data is to be sent
  * \param [in]	data	If non-null, the name.
  **************************************************************************************************/

 uint8_t TFT::send(TFT_MODE mode, uint8_t data)
 {
	if (CMD == mode)
	{
	 PIN_LOW(DC);
	}
	else
	{
	 PIN_HI(DC);
	}
	uint8_t retval = read(data);
	if (CMD == mode)
	{
	 delay_ms(1);
	}
	return retval;
 }

 void TFT::set_color(uint16_t color)
 {
	current_color = color;
 }

 void TFT::backlight(bool enable)
 {
	if (enable)
	{
	 PIN_HI(LED);
	}
	else
	{
	 PIN_LOW(LED);
	}
 }

 void TFT::set_frame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 {
	send(CMD, ILI9341_COLUMN_ADDR);
	send(DATA, x1 >> 8);
	send(DATA, x1 & 0x00ff);
	send(DATA, x2 >> 8);
	send(DATA, x2 & 0x00ff);
	send(CMD, ILI9341_PAGE_ADDR);
	send(DATA, y1 >> 8);
	send(DATA, y1 & 0x00ff);
	send(DATA, y2 >> 8);
	send(DATA, y2 & 0x00ff);
 }

 void TFT::plot_pixel(uint16_t x, uint16_t y)
 {
	set_frame(x, y, x, y);
	send(CMD, ILI9341_GRAM);
	send(DATA, current_color >> 8);
	send(DATA, current_color & 0xFF);
 }

 void TFT::configure()
 {
	PIN_LOW(RESET);
	delay_ms(500);
	PIN_HI(RESET);
	send(CMD, ILI9341_RESET);
	delay_ms(100);

	send(CMD, ILI9341_MAC);
	send(DATA, 0x48);
	send(CMD, ILI9341_PIXEL_FORMAT);
	send(DATA, 0x55);
	send(CMD, ILI9341_SLEEP_OUT);

	delay_ms(100);
	send(CMD, ILI9341_DISPLAY_ON);
	send(CMD, ILI9341_GRAM);
 }

 uint32_t TFT::reg_read(uint8_t command, uint8_t parameter)
 {
	send(CMD, 0xd9);
	send(DATA, 0x10 + parameter);
	send(CMD, command);
	uint16_t res = send(DATA, 0);
	PrintF("reg val: %x\n", res);
	return res;
 }

 bool TFT::check()
 {
	return 0 == reg_read(0xd3, 1) && 0x93 == reg_read(0xd3, 2) && 0x41 == reg_read(0xd3, 3);
 }

 void TFT::sleep(bool on)
 {
	send(CMD, on ? 0x10 : 0x11);
 }

 uint32_t TFT::status()
 {
	PrintF("Status: %x %x %x %x\n", reg_read(ILI9341_READ_STATUS, 2), reg_read(ILI9341_READ_STATUS, 3), reg_read(ILI9341_READ_STATUS, 4));
	return 0;
 }
}
