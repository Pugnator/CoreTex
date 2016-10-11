MCU?=STM32F10X_MD
TARGET?=STM32

PROJECTNAME:=CoreTex
GCCPRFX:=arm-none-eabi

AR:=$(GCCPRFX)-ar
GPP:=$(GCCPRFX)-g++
ASM:=$(GCCPRFX)-as
OCOPY:=$(GCCPRFX)-objcopy
STRIP:=$(GCCPRFX)-strip
SIZE:=$(GCCPRFX)-size
MKDIR_P = mkdir -p
MAKE:=make --always-make

LIBSDIR:=libs
OBJSDIR:=obj
OUTPUTDIR:=bin
MRI:=usedlibs.mri

CORELIB:=\
	src/core

AUXSRC:=\
	src/support	

DRIVERS:=\
	src/drivers

SUPPORT:=\
	src/support
	
FATFSDIR:=\
	src/drivers/storage/fat32

BLD?=DBG
export $(BLD)
ifeq ($(BLD), DBG)	
	CFLAGS:=-Og -g3 -DDEBUG	-ffunction-sections -fdata-sections
	LDFLAGS:=external/stdlib/Libraries/STM32F10x_StdPeriph_Driver/libstd.a -Wl,--gc-sections 
else
	CFLAGS:=-Os -ffunction-sections -fdata-sections
	LDFLAGS:=-Wl,--gc-sections 
endif

export CFLAGS
export LDFLAGS

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

####################################################

CFLAGS+=\
	--std=gnu++11 -Wall \
	-Iinclude -Isrc \
	-Iexternal/stdlib/Libraries/STM32F10x_StdPeriph_Driver/inc \
	-Iexternal/stdlib/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
	-D$(MCU) -D_GNU_SOURCE -D__STM32__\
	-D_REENT_SMALL -D_REENT_GLOBAL_ATEXIT \
	-Wno-write-strings \
	-fno-exceptions \
	-fno-builtin -nostdlib \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

LDFLAGS+=\
	-Tstm32_link.ld \
	-lm	

FATFSLIB:=$(LIBSDIR)/fat32.a

OBJ:=$(DRIVERSSRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(STARTUP:%.s=$(OBJSDIR)/%.o)
OBJ+=$(CORESRC:%.cc=$(OBJSDIR)/%.o)
OBJ+=$(SUPPORTSRC:%.cc=$(OBJSDIR)/%.o)

PROGOBJ:=$(SRC:%.cc=$(OBJSDIR)/%.o)

EXEC:=$(OUTPUTDIR)/$(PROJECTNAME).elf
FIRMWARE:=$(OUTPUTDIR)/$(PROJECTNAME)_linked.elf
LIBEXEC:=$(LIBSDIR)/$(PROJECTNAME).a

all: clean library firmware linked_firmware

linked_firmware: directories library $(FIRMWARE)

firmware: directories fatfs $(EXEC)

library: corelib drivers supportlib $(LIBEXEC)

$(OBJSDIR)/%.o: %.cc
	$(MKDIR_P) `dirname $@`
	$(GPP) -c -o $@ $< $(CFLAGS)

$(OBJSDIR)/%.o: %.s
	$(MKDIR_P) `dirname $@`
	$(ASM) $< -o $@

$(EXEC): $(OBJ)	$(PROGOBJ)
	$(GPP) -o $@ $^ $(LDFLAGS) $(FATFSLIB)
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
	$(MAKE) TARGET=$(TARGET) -C $(FATFSDIR);

corelib:
	for dir in $(CORELIB); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done

supportlib:
	for dir in $(SUPPORT); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done

drivers:
	for dir in $(DRIVERS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done


clean:
	rm -f $(EXEC) $(EXEC).bin $(LIBEXEC)
	rm -rf $(OUTPUTDIR) $(LIBSDIR)	$(OBJSDIR)

.PHONY: corelib
.PHONY: drivers
.PHONY: support