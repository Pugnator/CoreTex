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
#include "gfx.hpp"
#include <common.hpp>
#include <log.hpp>
#include <cstdlib>
#include <algorithm>

namespace Graphics
{

 void GFX::plot_pixel(uint16_t x, uint16_t y)
 {
	nss_low();
	set_frame(x, y, x, y);
	send(GLCD::CMD, ILI9341_GRAM);
	send(GLCD::DATA, current_color);
	nss_hi();
 }

 void GFX::plot_line(pixel p0, pixel p1)
 {
	uint16_t x0 = p0.first;
	uint16_t y0 = p0.second;
	uint16_t x1 = p1.first;
	uint16_t y1 = p1.second;

	if (x0 == x1)
	{
	 plot_vline(p0, p1);
	}
	else if (y0 == y1)
	{
	 plot_hline(p0, p1);
	}

	int32_t deltax = std::abs(x1 - x0);
	int32_t deltay = std::abs(y1 - y0);
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
	 plot_pixel(x, y);
	 error = error + deltaerr;
	 if (2 * error >= deltax)
	 {
		y = y + diry;
		error = error - deltax;
	 }
	}
 }

 void GFX::plot_hline(pixel p0, pixel p1)
 {
	nss_low();
	uint16_t x0 = p0.X;
	uint16_t y0 = p0.Y;
	uint16_t x1 = p1.first;
	if (x0 > x1)
	{
	 std::swap(x0, x1);
	}
	set_frame(x0, y0, x1, y0);
	send(GLCD::CMD, ILI9341_GRAM);
	for (uint16_t x = x0; x <= x1; ++x)
	{
	 send(GLCD::DATA, current_color);
	}
	nss_hi();
 }

 void GFX::plot_vline(pixel p0, pixel p1)
 {
	nss_low();
	uint16_t x0 = p0.X;
	uint16_t y0 = p0.Y;
	uint16_t y1 = p1.Y;
	if (y0 > y1)
	{
	 std::swap(y0, y1);
	}
	set_frame(x0, y0, x0, y1);
	send(GLCD::CMD, ILI9341_GRAM);
	for (uint16_t y = y0; y <= y1; ++y)
	{
	 send(GLCD::DATA, current_color);
	}
	nss_hi();
 }

 void GFX::plot_circle(pixel p0, uint16_t r)
 {
	uint16_t x0 = p0.X;
	uint16_t y0 = p0.Y;

	int x = 0;
	int y = r;
	int delta = 1 - 2 * r;
	int error = 0;
	while (y >= 0)
	{
	 plot_pixel(x0 + x, y0 + y);
	 plot_pixel(x0 + x, y0 - y);
	 plot_pixel(x0 - x, y0 + y);
	 plot_pixel(x0 - x, y0 - y);
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

 void GFX::plot_ellipse(pixel p0, uint16_t width, uint16_t height)
 {

 }

 void GFX::plot_polygon(points pts)
 {
	pixel f, s;
	for (points::iterator it = pts.begin(); it != pts.end();)
	{
	}
 }

 void GFX::plot_rect(pixel p0, uint16_t w, uint16_t h)
 {
	pixel p1 = std::make_pair(p0.X + w, p0.Y);
	pixel p2 = std::make_pair(p0.X + w, p0.Y + h);
	pixel p3 = std::make_pair(p0.X, p0.Y + h);

	plot_line(p0, p1);
	plot_line(p1, p2);
	plot_line(p2, p3);
	plot_line(p3, p0);
 }
}
