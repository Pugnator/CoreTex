#Setting file
DESCRIPTION:=mcu.ini

##########################
CPU:=STM32F103C8
MCU:=$(shell sed -n '/^family=/ {s///p;q;}' $(DESCRIPTION))
PROJECTNAME:=$(shell sed -n '/^name=/ {s///p;q;}' $(DESCRIPTION))
GCCPRFX:=$(shell sed -n '/^arch=/ {s///p;q;}' $(DESCRIPTION))
RTTVER:=$(shell sed -n '/^JLINK_VERSION=/ {s///p;q;}' $(DESCRIPTION))
SYSVVER:=$(shell sed -n '/^SYSVIEW_VERSION=/ {s///p;q;}' $(DESCRIPTION))

__USE_FATFS:=$(shell sed -n '/^__USE_FATFS=/ {s///p;q;}' $(DESCRIPTION))
__USE_TESTS:=$(shell sed -n '/^__USE_TESTS=/ {s///p;q;}' $(DESCRIPTION))
__USE_STDLIB:=$(shell sed -n '/^__USE_STDLIB=/ {s///p;q;}' $(DESCRIPTION))
__BUILDTYPE:=$(shell sed -n '/^__BUILDTYPE=/ {s///p;q;}' $(DESCRIPTION))
__USE_CONSOLE:=$(shell sed -n '/^__USE_CONSOLE=/ {s///p;q;}' $(DESCRIPTION))
__USE_MEMORY_DUMP:=$(shell sed -n '/^__USE_MEMORY_DUMP=/ {s///p;q;}' $(DESCRIPTION))
##########################

AR:=$(GCCPRFX)-ar
GPP:=$(GCCPRFX)-g++
ASM:=$(GCCPRFX)-as
OCOPY:=$(GCCPRFX)-objcopy
STRIP:=$(GCCPRFX)-strip
SIZE:=$(GCCPRFX)-size
READELF:=$(GCCPRFX)-readelf
CXXFILT:=$(GCCPRFX)-c++filt
SORT:=sort
AWK:=gawk
MKDIR_P = mkdir -p
MAKE:=make --always-make
JLINK:=jlink.exe
RM:=rm

LIBSDIR:=libs
OBJSDIR:=obj
OUTPUTDIR:=bin
MRI:=usedlibs.mri
STDLIB:=external/stdlib/F1X/Libraries/STM32F10x_StdPeriph_Driver/libstd.a

STMSTDLIBDIR:=\
	external/stdlib/F1X/Libraries/STM32F10x_StdPeriph_Driver
STDLIBSRC:=\
	external/stdlib/F1X/Libraries/STM32F10x_StdPeriph_Driver/src

CORELIB:=\
	src/core

SYSVIEWDIR:=\
	external/SEGGER_SYSTEMVIEW_$(SYSVVER)

RTTDIR:=\
	external/SEGGER_RTT_$(RTTVER)

UTILS:=\
	src/utils

DRIVERS:=\
	src/drivers

SUPPORT:=\
	src/support

TESTSDIR:=\
	src/tests

######################## SOURCES ###############################

SRC?=dummy.cc

DRIVERSSRC:=\
	$(DRIVERS)/file_transfer/xmodem/xmodem.cc \
	$(DRIVERS)/imaging/ov528/ov528.cc \
	$(DRIVERS)/positioning/gps/gps.cc \
	$(DRIVERS)/positioning/gps/gga.cc \
	$(DRIVERS)/positioning/gps/gll.cc \
	$(DRIVERS)/positioning/gps/gsv.cc \
	$(DRIVERS)/positioning/gps/parser.cc \
	$(DRIVERS)/positioning/gps/rmc.cc \
	$(DRIVERS)/positioning/gps/vtg.cc \
	$(DRIVERS)/storage/sdc.cc \
	$(DRIVERS)/storage/ramdisk.cc \
	$(DRIVERS)/storage/flash.cc \
	$(DRIVERS)/storage/eeprom.cc \
	$(DRIVERS)/wireless/modem/atcmd.cc \
	$(DRIVERS)/wireless/modem/bc417.cc \
	$(DRIVERS)/wireless/modem/gsm.cc \
	$(DRIVERS)/wireless/nrf24l01/nrf24.cc \
	$(DRIVERS)/userio/access/mfrc522/mfrc522.cc \
	$(DRIVERS)/userio/display/ili9341/hal.cc \
	$(DRIVERS)/userio/display/gfx/primitives.cc \
	$(DRIVERS)/userio/display/gfx/font.cc \
	$(DRIVERS)/userio/display/gfx/windows.cc \
	$(DRIVERS)/steppers/l6470.cc

ifeq ($(__USE_CONSOLE), 1)
DRIVERSSRC+=\
	$(DRIVERS)/userio/console/console.cc \
	$(DRIVERS)/userio/console/xprintf.cc
endif

CORESRC:=\
	$(CORELIB)/core.cc \
	$(CORELIB)/adc.cc \
	$(CORELIB)/pwm.cc \
	$(CORELIB)/gpio.cc \
	$(CORELIB)/iic.cc \
	$(CORELIB)/isr.cc \
	$(CORELIB)/isr_helper.cc \
	$(CORELIB)/rtc.cc \
	$(CORELIB)/spi.cc \
	$(CORELIB)/timers.cc \
	$(CORELIB)/usart.cc \
	$(CORELIB)/vmmu.cc

RTTSRC:=\
	$(RTTDIR)/Syscalls/SEGGER_RTT_Syscalls_GCC.c \
	$(RTTDIR)/RTT/SEGGER_RTT.c \
	$(RTTDIR)/RTT/SEGGER_RTT_printf.c	

SYSVIEWSRC:=\
	$(SYSVIEWDIR)/SEGGER_SYSVIEW.c \
	$(SYSVIEWDIR)/SEGGER_SYSVIEW_Config_NoOS.c	

STARTUP:=\
	$(CORELIB)/startup.s
	
ifeq ($(MCU), STM32F10X_MD)
	STARTUP:=$(CORELIB)/STM32F10X_MD.s
else
	STARTUP:=$(CORELIB)/STM32F10X_HD.s
endif

SUPPORTSRC:=\
	$(SUPPORT)/common.cc \
	$(SUPPORT)/compat.cc \
	$(SUPPORT)/log.cc \
	$(SUPPORT)/make_error.cc \
	$(SUPPORT)/queue.cc \
	$(SUPPORT)/morse.cc

XPRINTFSRC:=\
	$(SUPPORT)/xprintf/xprintf.c

FATFSSRC:=\
	$(DRIVERS)/storage/disk/fatdisk.cc \
	$(DRIVERS)/storage/disk/ff.cc \
	$(DRIVERS)/storage/disk/ffunicode.cc

UTILSSRC:=\
	$(UTILS)/tracker/gpx.cc \
	$(UTILS)/csv/csv.cc \
	$(UTILS)/xml/xml.cc

TESTSSRC:=\
	$(TESTSDIR)/core.cc

STMLIBSRC:=\
	$(STDLIBSRC)/misc.c           \
	$(STDLIBSRC)/stm32f10x_adc.c  \
	$(STDLIBSRC)/stm32f10x_bkp.c  \
	$(STDLIBSRC)/stm32f10x_can.c  \
	$(STDLIBSRC)/stm32f10x_cec.c  \
	$(STDLIBSRC)/stm32f10x_crc.c  \
	$(STDLIBSRC)/stm32f10x_dac.c  \
	$(STDLIBSRC)/stm32f10x_dbgmcu.c \
	$(STDLIBSRC)/stm32f10x_dma.c  \
	$(STDLIBSRC)/stm32f10x_exti.c \
	$(STDLIBSRC)/stm32f10x_flash.c \
	$(STDLIBSRC)/stm32f10x_fsmc.c \
	$(STDLIBSRC)/stm32f10x_gpio.c \
	$(STDLIBSRC)/stm32f10x_i2c.c  \
	$(STDLIBSRC)/stm32f10x_iwdg.c \
	$(STDLIBSRC)/stm32f10x_pwr.c  \
	$(STDLIBSRC)/stm32f10x_rcc.c  \
	$(STDLIBSRC)/stm32f10x_rtc.c  \
	$(STDLIBSRC)/stm32f10x_sdio.c \
	$(STDLIBSRC)/stm32f10x_spi.c  \
	$(STDLIBSRC)/stm32f10x_tim.c  \
	$(STDLIBSRC)/stm32f10x_usart.c \
	$(STDLIBSRC)/stm32f10x_wwdg.c

####################################################

ifeq ($(__BUILDTYPE), DEBUG)
	CXXFLAGS:=-Og -g3 -D__DEBUG=1
else ifeq ($(__BUILDTYPE), RELEASE)
	CXXFLAGS:=-flto -Os -s -D__DEBUG=0 -Werror
else
	$(error bad buildtype specified)
endif

ifeq ($(__USE_MEMORY_DUMP)), 1)
	CXXFLAGS+=-D__USE_MEMORY_DUMP=$(__USE_MEMORY_DUMP)
endif

CXXFLAGS+=\
	--std=gnu++14 -Wall \
	-Iinclude -Iinclude/errors -Isrc -Iinclude/support \
	-Iexternal/stdlib/Libraries/STM32F10x_StdPeriph_Driver/inc \
	-Iexternal/stdlib/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
	-Iexternal/stdlib/Libraries/CMSIS/CM3/CoreSupport \
	-D$(MCU) -D_GNU_SOURCE -D__STM32__\
	-D_REENT_SMALL -D_REENT_GLOBAL_ATEXIT \
	-Wno-write-strings \
	-fno-rtti \
	-fno-exceptions \
	-Dassert_param="" \
	-fpermissive \
	-fno-builtin \
	-fdata-sections \
	-ffunction-sections \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

ifeq ($(__USE_CONSOLE), 0)
	CXXFLAGS+=-D__USE_CONSOLE=1
else
	CXXFLAGS+=-D__USE_CONSOLE=0
endif

CXXFLAGS+=\
	-I$(RTTDIR)/RTT \
	-I$(SYSVIEWDIR)

ifeq ($(MCU), STM32F10X_MD)
	LDFLAGS+=-TSTM32F10X_MD.ld
else
	LDFLAGS+=-TSTM32F10X_HD.ld
endif

#-Wl,--wrap,__aeabi_unwind_cpp_pr0
LDFLAGS+=\
	-flto \
	-fno-exceptions \
	-fdata-sections \
	-ffunction-sections \
	-lm	-Wl,--gc-sections \
	-Wl,--cref \
	-Wl,-Map,$(FIRMWARE).map

##################OBJECT FILES###################


OBJ:=$(DRIVERSSRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(STARTUP:%.s=$(OBJSDIR)/%.o)
OBJ+=$(CORESRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(SUPPORTSRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(RTTSRC:%.c=$(OBJSDIR)/%.o)
OBJ+=$(XPRINTFSRC:%.c=$(OBJSDIR)/%.o)
OBJ+=$(UTILSSRC:%.cc=$(OBJSDIR)/%.o)


ifeq ($(__USE_FATFS),1)
OBJ+=$(FATFSSRC:%.cc=$(OBJSDIR)/%.o)
endif

ifeq ($(__USE_TESTS),1)
OBJ+=$(TESTSSRC:%.cc=$(OBJSDIR)/%.o)
endif

ifeq ($(__USE_STDLIB),1)
OBJ+=$(STMLIBSRC:%.c=$(OBJSDIR)/%.o)
endif

ifeq ($(__USE_SYSVIEW),1)
OBJ+=$(SYSVIEWSRC:%.c=$(OBJSDIR)/%.o)
endif

DEPS := $(OBJ:.o=.d)
-include $(DEPS)

PROGOBJ:=$(SRC:%.cc=$(OBJSDIR)/%.o)

######################OUTPUT###########################

FIRMWARE:=$(OUTPUTDIR)/$(PROJECTNAME).elf
LIBFIRMWARE:=$(LIBSDIR)/$(PROJECTNAME).a

######################TARGETS###########################
firmware: settings directories $(FIRMWARE)

flash:
	@echo -e "device STM32F103C8\r\nerase\r\nloadbin bin/CoreTex.elf.bin, 0x08000000\r\nr\r\ng\r\nq" > __temp.jlink
	$(JLINK) -if SWD -speed 4000 -CommanderScript __temp.jlink
	$(RM) -f __temp.jlink

reset:
	@echo -e "device $(CPU)\r\nr\r\ng\r\nq" > __temp.jlink
	$(JLINK) -if SWD -speed 4000 -CommanderScript __temp.jlink
	$(RM) -f __temp.jlink

erase:
	@echo -e "device $(CPU)\r\nerase\r\nq" > __temp.jlink
	$(JLINK) -if SWD -speed 4000 -CommanderScript __temp.jlink
	$(RM) -f __temp.jlink

$(OBJSDIR)/%.o: %.cc
	$(MKDIR_P) `dirname $@`
	$(GPP) -c $(CXXFLAGS) -MMD -o $@ $<
	$(SIZE) -d --format=berkeley $@

$(OBJSDIR)/%.o: %.c
	$(MKDIR_P) `dirname $@`
	$(GPP) -c $(CXXFLAGS) -MMD -o $@ $<
	$(SIZE) -d --format=berkeley $@

$(OBJSDIR)/%.o: %.s
	$(MKDIR_P) `dirname $@`
	$(ASM) $< -o $@
	$(SIZE) -d --format=berkeley $@

$(FIRMWARE): $(OBJ)	$(PROGOBJ)
	$(GPP) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) $(EXTLIB)
	$(OCOPY) -O binary $(FIRMWARE) $(FIRMWARE).bin
	@stat -c "Output size of <%n> is %s" $(FIRMWARE).bin
	@$(SIZE) --format=berkeley $(FIRMWARE)
	@-$(READELF) -s $@ | $(AWK) '$$3 != "0"' | $(AWK) '$$4 == "OBJECT"' | $(SORT) -k 3 -n -r | head -n 3 | $(CXXFILT)


directories:	$(OUTPUTDIR) $(OBJSDIR) $(LIBSDIR)

$(OUTPUTDIR):
	$(MKDIR_P) $(OUTPUTDIR)

$(OBJSDIR):
	$(MKDIR_P) $(OBJSDIR)

$(LIBSDIR):
	$(MKDIR_P) $(LIBSDIR)

clean:
	$(RM) -f $(STDLIB)
	$(RM) -rf $(OUTPUTDIR) $(LIBSDIR)	$(OBJSDIR)

settings:
	@echo -e "Project settings [$(PROJECTNAME)/$(MCU)]:"
#ifneq ($(__VMMU_POOL_SIZE), 0)
#	@echo -e "Memory pool size is $(__VMMU_POOL_SIZE)"
#endif

.PHONY: flash
.PHONY: erase
.PHONY: reset
.PHONY: settings