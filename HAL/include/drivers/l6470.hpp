#pragma once

#include <core/spi.hpp>
#include <common.hpp>

#define CONFIG_OSC_SEL                 0x000F // Mask for this bit field.
#define CONFIG_INT_16MHZ               0x0000 // Internal 16MHz, no output
#define CONFIG_INT_16MHZ_OSCOUT_2MHZ   0x0008 // Default; internal 16MHz, 2MHz output
#define CONFIG_INT_16MHZ_OSCOUT_4MHZ   0x0009 // Internal 16MHz, 4MHz output
#define CONFIG_INT_16MHZ_OSCOUT_8MHZ   0x000A // Internal 16MHz, 8MHz output
#define CONFIG_INT_16MHZ_OSCOUT_16MHZ  0x000B // Internal 16MHz, 16MHz output
#define CONFIG_EXT_8MHZ_XTAL_DRIVE     0x0004 // External 8MHz crystal
#define CONFIG_EXT_16MHZ_XTAL_DRIVE    0x0005 // External 16MHz crystal
#define CONFIG_EXT_24MHZ_XTAL_DRIVE    0x0006 // External 24MHz crystal
#define CONFIG_EXT_32MHZ_XTAL_DRIVE    0x0007 // External 32MHz crystal
#define CONFIG_EXT_8MHZ_OSCOUT_INVERT  0x000C // External 8MHz crystal, output inverted
#define CONFIG_EXT_16MHZ_OSCOUT_INVERT 0x000D // External 16MHz crystal, output inverted
#define CONFIG_EXT_24MHZ_OSCOUT_INVERT 0x000E // External 24MHz crystal, output inverted
#define CONFIG_EXT_32MHZ_OSCOUT_INVERT 0x000F // External 32MHz crystal, output inverted

// Configure the functionality of the external switch input
#define CONFIG_SW_MODE                 0x0010 // Mask for this bit.
#define CONFIG_SW_HARD_STOP            0x0000 // Default; hard stop motor on switch.
#define CONFIG_SW_USER                 0x0010 // Tie to the GoUntil and ReleaseSW
                                                    //  commands to provide jog function.
                                                    //  See page 25 of datasheet.

// Configure the motor voltage compensation mode (see page 34 of datasheet)
#define CONFIG_EN_VSCOMP               0x0020  // Mask for this bit.
#define CONFIG_VS_COMP_DISABLE         0x0000  // Disable motor voltage compensation.
#define CONFIG_VS_COMP_ENABLE          0x0020  // Enable motor voltage compensation.

// Configure overcurrent detection event handling
#define CONFIG_OC_SD                   0x0080  // Mask for this bit.
#define CONFIG_OC_SD_DISABLE           0x0000  // Bridges do NOT shutdown on OC detect
#define CONFIG_OC_SD_ENABLE            0x0080  // Bridges shutdown on OC detect

// Configure the slew rate of the power bridge output
#define CONFIG_POW_SR                  0x0300  // Mask for this bit field.
#define CONFIG_SR_180V_us              0x0000  // 180V/us
#define CONFIG_SR_290V_us              0x0200  // 290V/us
#define CONFIG_SR_530V_us              0x0300  // 530V/us

// Integer divisors for PWM sinewave generation
//  See page 32 of the datasheet for more information on this.
#define CONFIG_F_PWM_DEC               0x1C00      // mask for this bit field
#define CONFIG_PWM_MUL_0_625           (0x00<<10)
#define CONFIG_PWM_MUL_0_75            (0x01<<10)
#define CONFIG_PWM_MUL_0_875           (0x02<<10)
#define CONFIG_PWM_MUL_1               (0x03<<10)
#define CONFIG_PWM_MUL_1_25            (0x04<<10)
#define CONFIG_PWM_MUL_1_5             (0x05<<10)
#define CONFIG_PWM_MUL_1_75            (0x06<<10)
#define CONFIG_PWM_MUL_2               (0x07<<10)

// Multiplier for the PWM sinewave frequency
#define CONFIG_F_PWM_INT               0xE000     // mask for this bit field.
#define CONFIG_PWM_DIV_1               (0x00<<13)
#define CONFIG_PWM_DIV_2               (0x01<<13)
#define CONFIG_PWM_DIV_3               (0x02<<13)
#define CONFIG_PWM_DIV_4               (0x03<<13)
#define CONFIG_PWM_DIV_5               (0x04<<13)
#define CONFIG_PWM_DIV_6               (0x05<<13)
#define CONFIG_PWM_DIV_7 (0x06<<13)

#define dSPIN_NOP             0x00
#define dSPIN_SET_PARAM       0x00
#define dSPIN_GET_PARAM       0x20
#define dSPIN_RUN             0x50
#define dSPIN_STEP_CLOCK      0x58
#define dSPIN_MOVE            0x40
#define dSPIN_GOTO            0x60
#define dSPIN_GOTO_DIR        0x68
#define dSPIN_GO_UNTIL        0x82
#define dSPIN_RELEASE_SW      0x92
#define dSPIN_GO_HOME         0x70
#define dSPIN_GO_MARK         0x78
#define dSPIN_RESET_POS       0xD8
#define dSPIN_RESET_DEVICE    0xC0
#define dSPIN_SOFT_STOP       0xB0
#define dSPIN_HARD_STOP       0xB8
#define dSPIN_SOFT_HIZ        0xA0
#define dSPIN_HARD_HIZ        0xA8
#define dSPIN_GET_STATUS 0xD0

#define DSRST_PIN A,3,SPEED_50MHz

class l6470: public Spi
{
public:
 l6470(short ch): Spi(ch)
{
	 PIN_OUT_PP(DSRST_PIN);
	 PIN_LOW(DSRST_PIN);
	 delay_ms(500);
	 PIN_HI(DSRST_PIN);
	 delay_ms(500);
	 read(dSPIN_SET_PARAM | 0x18);
	 read(CONFIG_PWM_DIV_1 | CONFIG_PWM_MUL_2 | CONFIG_SR_290V_us| CONFIG_OC_SD_DISABLE | CONFIG_VS_COMP_DISABLE | CONFIG_SW_HARD_STOP | CONFIG_INT_16MHZ);

	 read(dSPIN_SET_PARAM | 0x0A);
	 read(0xFF);

	 read(dSPIN_RUN | 1);
	 uint32_t spd = calc(25.173);
	 read(spd >> 16);
	 read(spd >> 8);
	 read(spd & 0xFF);
};
 ~l6470(){};

 void run(float speed);
private:
 uint32_t calc(float stepsPerSec);
};
