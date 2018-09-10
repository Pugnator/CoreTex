#pragma once
#include <drivers/ili9341.hpp>
#include <list>
#include <utility>

namespace Graphics
{
 typedef std::pair<uint16_t, uint16_t> pixel;

 typedef std::list<pixel> points;

 class GFX : public GLCD::TFT
 {
 public:
	GFX(char channel)
		: GLCD::TFT (channel)
	{
	 max_x = 240;
	 max_y = 320;
	 brush = 1;
	}
	;
	~GFX()
	{
	}
	;

 public:
	/* Basic */
	void line(pixel p0, pixel p1);
	void circle(pixel p0, uint16_t r);
	void point(pixel p0, uint16_t r);
	void arc(pixel p0, uint16_t r, uint16_t a1, uint16_t a2);
	void polygon(points pts);

 private:
	uint16_t max_x;
	uint16_t max_y;
	uint8_t brush;
 };
}
