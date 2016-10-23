DESCRIPTION:=mcu.ini

MCU:=$(shell sed -n '/^family=/ {s///p;q;}' $(DESCRIPTION))
PROJECTNAME:=$(shell sed -n '/^name=/ {s///p;q;}' $(DESCRIPTION))
GCCPRFX:=$(shell sed -n '/^arch=/ {s///p;q;}' $(DESCRIPTION))

AR:=$(GCCPRFX)-ar
GPP:=$(GCCPRFX)-g++
ASM:=$(GCCPRFX)-as
OCOPY:=$(GCCPRFX)-objcopy
STRIP:=$(GCCPRFX)-strip
SIZE:=$(GCCPRFX)-size
MKDIR_P = mkdir -p
MAKE:=make --always-make
JLINK:=jlink.exe
RM:=rm

LIBSDIR:=libs
OBJSDIR:=obj
OUTPUTDIR:=bin
MRI:=usedlibs.mri
STDLIB:=external/stdlib/Libraries/STM32F10x_StdPeriph_Driver/libstd.a
STMSTDLIBDIR:=\
	external/stdlib/Libraries/STM32F10x_StdPeriph_Driver
STDLIBSRC:=\
	external/stdlib/Libraries/STM32F10x_StdPeriph_Driver/src

CORELIB:=\
	src/core

AUXSRC:=\
	src/support	

DRIVERS:=\
	src/drivers

SUPPORT:=\
	src/support
	
FATFSDIR:=\
	src/drivers/storage/fatfs
	
TESTSDIR:=\
	src/tests

BLD?=DBG
ifeq ($(BLD), DBG)	
	CFLAGS:=-Og -g3
else
	CFLAGS:=-Os -s
endif

######################## SOURCES ###############################

SRC+=\
	src/main.cc

DRIVERSSRC:=\
	$(DRIVERS)/file_transfer/xmodem/xmodem.cc \
	$(DRIVERS)/generic/driver.cc \
	$(DRIVERS)/imaging/ov528/ov528.cc \
	$(DRIVERS)/positioning/gps/gga.cc \
	$(DRIVERS)/positioning/gps/gps.cc \
	$(DRIVERS)/positioning/gps/rmc.cc \
	$(DRIVERS)/positioning/gps/vtg.cc \
	$(DRIVERS)/storage/mmc/mmc.cc \
	$(DRIVERS)/storage/mmc/disk.cc \
	$(DRIVERS)/storage/ramdisk.cc \
	$(DRIVERS)/storage/flash.cc \
	$(DRIVERS)/userio/console/console.cc \
	$(DRIVERS)/userio/console/xprintf.cc \
	$(DRIVERS)/wireless/bc470/bc470.cc \
	$(DRIVERS)/wireless/gsm/atcmd.cc \
	$(DRIVERS)/wireless/gsm/gsm.cc \
	$(DRIVERS)/wireless/nrf24l01/nrf24.cc

CORESRC:=\
	$(CORELIB)/adc.cc \
	$(CORELIB)/core.cc \
	$(CORELIB)/gpio.cc \
	$(CORELIB)/i2c.cc \
	$(CORELIB)/isr.cc \
	$(CORELIB)/isr_helper.cc \
	$(CORELIB)/rtc.cc \
	$(CORELIB)/spi.cc \
	$(CORELIB)/timers.cc \
	$(CORELIB)/usart.cc \
	$(CORELIB)/vmmu.cc

STARTUP:=\
	$(CORELIB)/startup.s

SUPPORTSRC:=\
	$(SUPPORT)/common.cc \
	$(SUPPORT)/compat.cc \
	$(SUPPORT)/log.cc \
	$(SUPPORT)/queue.cc \
	$(SUPPORT)/morse.cc
	
FATFSSRC:=\
	$(FATFSDIR)/ff.c

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

CXXFLAGS+=\
	--std=gnu++11 -Wall \
	-Iinclude -Isrc \
	-Iexternal/stdlib/Libraries/STM32F10x_StdPeriph_Driver/inc \
	-Iexternal/stdlib/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
	-Iexternal/stdlib/Libraries/CMSIS/CM3/CoreSupport \
	-D$(MCU) -D_GNU_SOURCE -D__STM32__\
	-D_REENT_SMALL -D_REENT_GLOBAL_ATEXIT \
	-Wno-write-strings \
	-fno-exceptions \
	-Dassert_param="" \
	-fno-builtin -nostdlib \
	-ffunction-sections -fdata-sections \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

LDFLAGS+=\
	-Tstm32_link.ld \
	-lm	-Wl,--gc-sections

OBJ:=$(DRIVERSSRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(STARTUP:%.s=$(OBJSDIR)/%.o)
OBJ+=$(CORESRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(SUPPORTSRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(FATFSSRC:%.c=$(OBJSDIR)/%.o)
OBJ+=$(TESTSSRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(STMLIBSRC:%.c=$(OBJSDIR)/%.o)

DEPS := $(OBJ:.o=.d)

-include $(DEPS)

PROGOBJ:=$(SRC:%.cc=$(OBJSDIR)/%.o)

EXEC:=$(OUTPUTDIR)/$(PROJECTNAME).elf
FIRMWARE:=$(OUTPUTDIR)/$(PROJECTNAME)_linked.elf
LIBEXEC:=$(LIBSDIR)/$(PROJECTNAME).a

all: clean directories stm_stdlib library firmware linked_firmware

linked_firmware: directories library $(FIRMWARE)

firmware: directories $(EXEC)

flash: 
	@echo -e "device STM32F103C8\r\nerase\r\nloadbin ../bin/CoreTex.elf.bin, 0x08000000\r\nr\r\ng\r\nq" > __temp.jlink
	$(JLINK) -if SWD -speed 4000 -CommanderScript __temp.jlink
	$(RM) -f __temp.jlink
	
reset:
	@echo -e "device STM32F103C8\r\nr\r\ng\r\nq" > __temp.jlink
	$(JLINK) -if SWD -speed 4000 -CommanderScript __temp.jlink
	$(RM) -f __temp.jlink

erase:
	@echo -e "device STM32F103C8\r\nerase\r\nq" > __temp.jlink
	$(JLINK) -if SWD -speed 4000 -CommanderScript __temp.jlink
	$(RM) -f __temp.jlink

library: corelib drivers supportlib $(LIBEXEC)

$(OBJSDIR)/%.o: %.cc
	$(MKDIR_P) `dirname $@`	
	$(GPP) -c $(CXXFLAGS) -MMD -o $@ $< 
	
$(OBJSDIR)/%.o: %.c
	$(MKDIR_P) `dirname $@`	
	$(GPP) -c $(CXXFLAGS) -MMD -o $@ $< 

$(OBJSDIR)/%.o: %.s
	$(MKDIR_P) `dirname $@`
	$(ASM) $< -o $@

$(EXEC): $(OBJ)	$(PROGOBJ)
	$(GPP) -o $@ $^ $(LDFLAGS)
	$(OCOPY) -O binary $(EXEC) $(EXEC).bin
	@stat -c "Output size of <%n> is %s" $(EXEC).bin
	@$(SIZE) --format=berkeley $(EXEC)

$(FIRMWARE): $(PROGOBJ)
	$(GPP) -o $@ $^ $(LDFLAGS) $(LIBEXEC)
	$(OCOPY) -O binary $(FIRMWARE) $(FIRMWARE).bin
	@stat -c "Output size of <%n> is %s" $(FIRMWARE).bin
	@$(SIZE) --format=berkeley $(FIRMWARE)

$(LIBEXEC): $(MRI)
	$(AR) M <$^
	@stat -c "Output size of <%n> is %s" $(LIBEXEC)

directories:	$(OUTPUTDIR) $(OBJSDIR) $(LIBSDIR)

$(OUTPUTDIR):
	$(MKDIR_P) $(OUTPUTDIR)

$(OBJSDIR):
	$(MKDIR_P) $(OBJSDIR)

$(LIBSDIR):
	$(MKDIR_P) $(LIBSDIR)

fatfs:
	$(MAKE) -C $(FATFSDIR);
	
stm_stdlib:
	for dir in $(STMSTDLIBDIR); do \
		$(MAKE) BLD=$(BLD) -C $$dir; \
	done

corelib:
	for dir in $(CORELIB); do \
		$(MAKE) BLD=$(BLD) -C $$dir; \
	done

supportlib:
	for dir in $(SUPPORT); do \
		$(MAKE) BLD=$(BLD) -C $$dir; \
	done

drivers:
	for dir in $(DRIVERS); do \
		$(MAKE) BLD=$(BLD) -C $$dir; \
	done

clean:
	$(RM) -f $(STDLIB)
	$(RM) -rf $(OUTPUTDIR) $(LIBSDIR)	$(OBJSDIR)

.PHONY: stm_stdlib
.PHONY: corelib
.PHONY: drivers
.PHONY: support
.PHONY: flash
.PHONY: erase
.PHONY: reset