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

MAKE:=make --always-make 

SUBDIRS=\
src/drivers/storage/mmc/fat32 \
src/core

BLD?=DBG
ifeq ($(BLD), DBG)	
	CFLAGS:=-Og -g3 -DDEBUG	-ffunction-sections -fdata-sections
	LDFLAGS+=external/stdlib/Libraries/STM32F10x_StdPeriph_Driver/libstd.a -Wl,--gc-sections 
else
   CFLAGS:=-Os -s -ffunction-sections -fdata-sections
   LDFLAGS+=-Wl,--gc-sections 
endif

SRC+=\
	src/other/compat.cc \
	src/other/log.cc \
	src/other/common.cc \
	src/other/morse.cc \
	src/drivers/generic/driver.cc \
	src/drivers/storage/mmc/mmc.cc \
	src/drivers/wireless/gsm/gsm.cc \
	src/drivers/wireless/gsm/atcmd.cc \
	src/drivers/userio/console/xprintf.cc \
	src/drivers/userio/console/console.cc \
	src/drivers/imaging/ov528/ov528.cc \
	src/drivers/positioning/gps/gps.cc \
	src/drivers/positioning/gps/gga.cc \
	src/drivers/positioning/gps/vtg.cc \
	src/drivers/positioning/gps/rmc.cc \
	src/drivers/file_transfer/xmodem/xmodem.cc \
	src/drivers/wireless/nrf24l01/nrf24.cc \
	src/drivers/wireless/bc470/bc470.cc	

MAINSRC:=\
	src/main.cc

CORELIBS +=\
	src/core/libcore.a \
	src/drivers/storage/mmc/fat32/libfat32.a


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
	-lm \
	$(CORELIBS) \
	$(LIBEXEC)
	
OBJ:=$(SRC:%.cc=%.o)

EXEC:=$(PROJECTNAME).elf
LIBEXEC:=$(PROJECTNAME).a

all: subdirs $(LIBEXEC)

bin: $(EXEC)

%.o: %.cc
	$(GPP) -c -o $@ $< $(CFLAGS)

$(EXEC): src/main.o
	$(GPP) -o $@ $^ $(LDFLAGS)
	$(OCOPY) -O binary $(EXEC) $(EXEC).bin
	@stat -c "Output size of <%n> is %s" $(EXEC).bin
	@$(SIZE) --format=berkeley $(EXEC)


$(LIBEXEC): $(OBJ) $(CORELIBS)
	$(AR) rcs $@ $^
	@stat -c "Output size of <%n> is %s" $(LIBEXEC)

subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done


clean:
	rm -f $(EXEC).* $(EXEC)
	/usr/bin/find . -name "*.o" -type f -print0 | xargs -0 /bin/rm -f

	for dir in $(SUBDIRS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir clean; \
	done

.PHONY: subdirs


