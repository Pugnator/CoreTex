#pragma once
#include "core/spi.hpp"

#define X first
#define Y second

#define BLACK                       0x0000
#define BLUE                        0x001F
#define RED                         0xF800
#define GREEN                       0x07E0
#define CYAN                        0x07FF
#define MAGENTA                     0xF81F
#define YELLOW                      0xFFE0
#define WHITE                       0xFFFF

namespace GLCD
{

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

 using namespace IO;

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
	 lowspeed();
	 max_x = 240;
	 max_y = 320;


	 LED_pin.reset(new GPIO_pin({ PORTA, P1, IOSPEED_10MHz, OUT_ALT_PP }));
	 RST_pin.reset(new GPIO_pin({ PORTA, P3, IOSPEED_50MHz, OUT_PP }));
	 DC_pin.reset(new GPIO_pin({ PORTA, P2, IOSPEED_50MHz, OUT_PP }));


	 highspeed();
	 configure();
	 go16bit();
	 current_color = WHITE; // white by default
	 backlight(80);
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

	void backlight(uint8_t brightness);

 protected:
	uint32_t reg_read(uint8_t command, uint8_t parameter);
	void configure();
	uint8_t send16(TFT_MODE mode, uint16_t data);
	uint8_t send8(TFT_MODE mode, uint8_t data);

	uint16_t max_x;
	uint16_t max_y;
	uint16_t current_color;

	std::unique_ptr<IO::GPIO_pin> LED_pin;
	std::unique_ptr<IO::GPIO_pin> RST_pin;
	std::unique_ptr<IO::GPIO_pin> DC_pin;
 };
}
