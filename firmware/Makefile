CC:=arm-none-eabi-gcc
GPP:=arm-none-eabi-g++
OCOPY:=arm-none-eabi-objcopy
STRIP:=arm-none-eabi-strip
DEL:=rm

STARTUP:=startup_stm32f10x_md.s
CORE:=src/core
DRIVERS:=src/drivers
AUX:=src/aux

SUBDIRS=src/fat32 nmea_parser

SRC:=\
	src/main.cc \
	$(CORE)/usart.cc \
	$(CORE)/config.cc \
	$(CORE)/isr.cc \
	$(AUX)/compat.cc \
	$(AUX)/log.cc \
	$(CORE)/spi.cc \
	$(CORE)/i2c.cc \
	$(CORE)/timers.cc \
	$(CORE)/rtc.cc \
	$(DRIVERS)/gsm.cc \
	$(DRIVERS)/sdcard.cc \
	$(AUX)/common.cc \
	$(AUX)/morse.cc \
	$(CORE)/$(STARTUP)

CFLAGS+=\
-O0 -g3 --std=c++11 -Wall \
-Iinclude -Isrc/fat32 -Inmea_parser \
-DSTM32F10X_MD -D_GNU_SOURCE -DDEBUG \
-Wno-write-strings \
-fno-exceptions -ffunction-sections -fdata-sections -fno-builtin \
-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

LDFLAGS+=-Tstm32_link.ld -nostartfiles src/fat32/libfat32.a -lm nmea_parser/libnmea.a

#OBJ=$(join $(addsuffix src/, $(dir $(SRC))), $(notdir $(SRC:.cc=.o)))
OBJ=$(SRC:%.cc=%.o)

EXEC:=program

all: subdirs $(EXEC).elf

%.o: %.cc
	$(GPP) -c -o $@ $< $(CFLAGS)

$(EXEC).elf: $(OBJ)
	$(GPP) -o $@ $^ $(LDFLAGS)
	$(OCOPY) -O binary $(EXEC).elf $(EXEC).bin


subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

clean:
	$(DEL) -f $(EXEC).*
	find -type f -regex ".*/.*\.\(o\|d\\)" -delete
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done

install:

debug:


.PHONY: subdirs

