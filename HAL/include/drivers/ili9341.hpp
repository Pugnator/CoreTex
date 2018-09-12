#pragma once
#include "core/spi.hpp"

namespace GLCD
{

#define LED A,1,SPEED_50MHz
#define RESET A,3,SPEED_50MHz
#define DC A,2,SPEED_50MHz

//Commands
#define ILI9341_RESET				0x01
#define ILI9341_READ_STATUS				0x09
#define ILI9341_SLEEP_OUT			0x11
#define ILI9341_NORMAL_MODE			0x13
#define ILI9341_GAMMA				0x26
#define ILI9341_DISPLAY_OFF			0x28
#define ILI9341_DISPLAY_ON			0x29
#define ILI9341_COLUMN_ADDR			0x2A
#define ILI9341_PAGE_ADDR			0x2B
#define ILI9341_GRAM				0x2C
#define ILI9341_MAC			        0x36
#define ILI9341_PIXEL_FORMAT        0x3A
#define ILI9341_WDB			    	0x51
#define ILI9341_WCD				    0x53
#define ILI9341_RGB_INTERFACE       0xB0
#define ILI9341_FRC				    0xB1
#define ILI9341_BPC				    0xB5
#define ILI9341_DFC				    0xB6
#define ILI9341_POWER1				0xC0
#define ILI9341_POWER2				0xC1
#define ILI9341_VCOM1				0xC5
#define ILI9341_VCOM2				0xC7
#define ILI9341_POWERA				0xCB
#define ILI9341_POWERB				0xCF
#define ILI9341_PGAMMA				0xE0
#define ILI9341_NGAMMA				0xE1
#define ILI9341_DTCA				0xE8
#define ILI9341_DTCB				0xEA
#define ILI9341_POWER_SEQ			0xED
#define ILI9341_3GAMMA_EN			0xF2
#define ILI9341_INTERFACE			0xF6
#define ILI9341_PRC				    0xF7

 typedef enum
 {
	CMD, DATA
 } TFT_MODE;

 class TFT : public Spi
 {
 public:
	TFT(char channel)
		: Spi(channel)
	{
	 max_x = 240;
	 max_y = 320;
	 PIN_OUT_PP(LED);
	 PIN_OUT_PP(RESET);
	 PIN_OUT_PP(DC);
	 lowspeed();
	 configure();
	 highspeed();
	 current_color = 0xFFFF; // white by default
	 backlight(true);
	}
	;
	~TFT()
	{
	}
	;

 public:
	bool check();
	uint32_t status();
	void sleep(bool on);

 public:
	void set_color(uint16_t color);
	void clear_display();
	void fill_display(uint16_t color);
	void set_frame(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
	void plot_pixel(uint16_t x, uint16_t y);

	void backlight(bool enable);

 protected:
	uint32_t reg_read(uint8_t command, uint8_t parameter);
	void configure();
	uint8_t send(TFT_MODE mode, uint8_t data);

	uint16_t max_x;
	uint16_t max_y;
	uint16_t current_color;
 };
}
