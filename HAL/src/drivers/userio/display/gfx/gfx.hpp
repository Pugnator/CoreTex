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
	 brush = 1;
	}
	;
	~GFX()
	{
	}
	;

 public:
	/* Basic */
	void plot_line(pixel p0, pixel p1);
	void plot_circle(pixel p0, uint16_t r);  
  void plot_ellipse(pixel p0, uint16_t w, uint16_t h);	
	void plot_arc(pixel p0, uint16_t r, uint16_t a1, uint16_t a2);
  void plot_rect(pixel p0, uint16_t w, uint16_t h);
	void plot_polygon(points pts);

 private:
	uint8_t brush;
 };
}
