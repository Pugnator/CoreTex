/**
 *
 * @file   draw.cc
 * @author Lavrentiy Ivanov (ookami@mail.ru)
 * @copyright Copyright 2015 Lavrentiy Ivanov. All rights reserved
 * @license This project is released under the GPL 2 license.
 * @brief Draw methods
 *
 */

#include "gfx.hpp"
#include <common.hpp>
#include <log.hpp>
#include <cstdlib>

namespace Graphics
{
 void GFX::line(pixel p0, pixel p1)
 {
	uint16_t x0 = p0.first;
	uint16_t y0 = p0.second;
	uint16_t x1 = p1.first;
	uint16_t y1 = p1.second;
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
	 set_pixel(x, y);
	 error = error + deltaerr;
	 if (2 * error >= deltax)
	 {
		y = y + diry;
		error = error - deltax;
	 }
	}
 }

 void GFX::circle(pixel p0, uint16_t r)
 {
	uint16_t x0 = p0.first;
	uint16_t y0 = p0.second;

	int x = 0;
	int y = r;
	int delta = 1 - 2 * r;
	int error = 0;
	while (y >= 0)
	{
	 set_pixel(x0 + x, y0 + y);
	 set_pixel(x0 + x, y0 - y);
	 set_pixel(x0 - x, y0 + y);
	 set_pixel(x0 - x, y0 - y);
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

 void GFX::polygon(points pts)
 {
	pixel f, s;
	for (points::iterator it = pts.begin(); it != pts.end();)
	{
	 line(s, *it);
	 f = *it;
	 ++it;
	 if (it != pts.end())
	 {
		s = *it;
	 }
	 else
	 {
		set_pixel(f.first, f.second);
		break;
	 }
	 line(f, s);
	}
 }
}
