/**
 *
 * @file   draw.cc
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @copyright Copyright 2015 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the GPL 2 license.
 * @brief Draw methods
 *
 */

#include <drivers/ili9341.hpp>
#include <common.hpp>
#include <log.hpp>
#include <cstdlib>

namespace TFT
{
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

 void TFT::line (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
 {
	int32_t deltax = std::abs (x1 - x0);
	int32_t deltay = std::abs (y1 - y0);
	int32_t error = 0;
	int32_t deltaerr = deltay;
	int32_t y = y0;
	int32_t diry = y1 - y0;
	if (diry > 0)
	{
	 diry = 1;
	}
	if (diry < 0)
	{
	 diry = -1;
	}

	for (int x = x0; x <= x1; ++x)
	{
	 set_pixel (x, y);
	 error = error + deltaerr;
	 if (2 * error >= deltax)
	 {
		y = y + diry;
		error = error - deltax;
	 }
	}
 }

 void TFT::circle (uint16_t x0, uint16_t y0, uint16_t r)
 {
	int x = 0;
	int y = r;
	int delta = 1 - 2 * r;
	int error = 0;
	while (y >= 0)
	{
	 set_pixel (x0 + x, y0 + y);
	 set_pixel (x0 + x, y0 - y);
	 set_pixel (x0 - x, y0 + y);
	 set_pixel (x0 - x, y0 - y);
	 error = 2 * (delta + y) - 1;
	 if ((delta < 0) && (error <= 0))
	 {
		delta += 2 * ++x + 1;
		continue;
	 }
	 else if ((delta > 0) && (error > 0))
	 {
		delta -= 2 * --y + 1;
		continue;
	 }
	 delta += 2 * (++x - y--);
	}
 }
}
