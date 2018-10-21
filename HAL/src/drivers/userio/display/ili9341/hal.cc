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
#include "../gfx/font.hpp"
#include <core/gpio.hpp>
#include <common.hpp>
#include <log.hpp>

namespace GLCD
{

 void TFT::clear_display()
 {
	uint8_t cc = current_color;
	fill_display(0);
	set_color(cc);
 }

 void TFT::fill_display(uint16_t color)
 {
	set_color(color);
	nss_low();
	set_frame(0, 0, 239, 319);
	send16(CMD, ILI9341_GRAM);
	for (uint16_t x = 0; x < 240; ++x)
	{
	 for (uint16_t y = 0; y < 320; ++y)
	 {
		send16(DATA, color);
	 }
	}
	nss_hi();
 }

 /**********************************************************************************************//**
  * \fn	uint8_t send16 (TFT_MODE mode, uint8_t data)
  *
  * \brief	Send the data to the display
  *
  * \param [in] mode Command or data is to be sent
  * \param [in]	data	If non-null, the name.
  **************************************************************************************************/

 uint8_t TFT::send16(TFT_MODE mode, uint16_t data)
 {
	if (CMD == mode)
	{
	 DC_pin->hi();
	 DC_pin->low();
	}
	else
	{
	 DC_pin->low();
	 DC_pin->hi();
	}
	uint8_t retval = read_no_nss(data);
	return retval;
 }

 uint8_t TFT::send8(TFT_MODE mode, uint8_t data)
 {
	if (CMD == mode)
	{
	 DC_pin->low();
	}
	else
	{
	 DC_pin->hi();
	}
	uint8_t retval = read(data);
	return retval;
 }

 void TFT::set_color(uint16_t color)
 {
	current_color = color;
 }

 void TFT::backlight(uint8_t brightness)
 {
	if (0 < brightness)
	{
	 LED_pin->pwm(true);
	 LED_pin->pwm_duty(brightness);
	}
	else
	{
	 LED_pin->pwm(false);
	}
 }

 void TFT::set_frame(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
 {
	send16(CMD, ILI9341_COLUMN_ADDR);
	send16(DATA, x1);
	send16(DATA, x2);
	send16(CMD, ILI9341_PAGE_ADDR);
	send16(DATA, y1);
	send16(DATA, y2);
 }

 void TFT::configure()
 {
	RST_pin->low();
	delay_ms(500);
	RST_pin->hi();

	nss_low();
	send16(CMD, ILI9341_RESET);
	delay_ms(100);

	send16(CMD, ILI9341_MAC);
	send16(DATA, 0x48);
	send16(CMD, ILI9341_PIXEL_FORMAT);
	send16(DATA, 0x55);

	send16(CMD, ILI9341_SLEEP_OUT);

	delay_ms(100);
	send16(CMD, ILI9341_DISPLAY_ON);
	send16(CMD, ILI9341_GRAM);
	nss_hi();
 }

 uint32_t TFT::reg_read(uint8_t command, uint8_t parameter)
 {
	send8(CMD, 0xd9);
	send8(DATA, 0x10 + parameter);
	send8(CMD, command);
	uint8_t res = 0;
	PrintF("regRead: 0x%X\r\n", send8(DATA, 0));
	PrintF("regRead: 0x%X\r\n", send8(DATA, 0));
	PrintF("regRead: 0x%X\r\n", send8(DATA, 0));
	PrintF("regRead: 0x%X\r\n", send8(DATA, 0));
	return res;
 }

 bool TFT::check()
 {
	return 0 == reg_read(0xd3, 1) && 0x93 == reg_read(0xd3, 2) && 0x41 == reg_read(0xd3, 3);
 }

 void TFT::sleep(bool on)
 {
	nss_low();
	send16(CMD, on ? 0x10 : 0x11);
	nss_hi();
 }

 uint32_t TFT::status()
 {
	PrintF("Status: %x %x %x %x\n", reg_read(ILI9341_READ_STATUS, 2), reg_read(ILI9341_READ_STATUS, 3), reg_read(ILI9341_READ_STATUS, 4));
	return 0;
 }
}
