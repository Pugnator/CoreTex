#pragma once
#include <drivers/ili9341.hpp>
#include <list>
namespace Graphics
{
  typedef struct
  {
  }point;
  
  typedef std::list<point> points;
  class GFX : public GLCD::TFT
  {
    public:
    GFX (char channel) : GLCD::TFT (channel){};
    ~GFX(){};
    
    public:    
    void line (uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
    void circle(uint16_t x0, uint16_t y0, uint16_t r);
    
    private:
  };
}