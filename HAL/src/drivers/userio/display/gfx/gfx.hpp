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
		: GLCD::TFT(channel)
	{
	 brush = 1;
	 char_offset = 0;
	}
	;
	~GFX()
	{
	}
	;

 public:
	/* Basic */
	void plot_pixel(uint16_t x, uint16_t y);
	void plot_line(pixel p0, pixel p1);
	void plot_hline(pixel p0, pixel p1);
	void plot_vline(pixel p0, pixel p1);
	void plot_circle(pixel p0, uint16_t r);
	void plot_ellipse(pixel p0, uint16_t w, uint16_t h);
	void plot_arc(pixel p0, uint16_t r, uint16_t a1, uint16_t a2);
	void plot_rect(pixel p0, uint16_t w, uint16_t h);
	void plot_polygon(points pts);
	/* Fonts */
	void plot_char(char c, uint8_t x0, uint8_t y0);
	void print(const char* text, uint8_t x0, uint8_t y0);
	void set_char_offset(uint8_t offset);
 private:
	uint8_t brush;
	uint8_t char_offset;
 };
}
