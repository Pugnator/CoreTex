MCU?=STM32F10X_MD
TARGET?=STM32

ifeq ($(TARGET), STM32)
	CC:=arm-none-eabi-gcc
	GPP:=arm-none-eabi-g++
	OCOPY:=arm-none-eabi-objcopy
	STRIP:=arm-none-eabi-strip
	EXEC:=bin/program
else ifeq ($(TARGET), Linux)
   	CC:=gcc
	GPP:=g++
	OCOPY:=objcopy
	STRIP:=strip
	EXEC:=bin/test_program
endif

DEL:=rm
MAKE:=make

SUBDIRS=src/fat32 nmea_parser src/hal

ifeq ($(TARGET), STM32)
	SRC:=\
	src/main.cc \
	src/aux/compat.cc \
	src/aux/log.cc \
	src/drivers/gsm.cc \
	src/drivers/sdcard.cc \
	src/aux/common.cc \
	src/aux/morse.cc
else ifeq ($(TARGET), Linux)
	SRC:=\
	src/linux/main.cc
endif

ifeq ($(TARGET), STM32)
	CFLAGS+=\
	-O0 -g3 --std=gnu++11 -Wall \
	-Iinclude -Isrc/fat32 -Inmea_parser \
	-D$(MCU) -D_GNU_SOURCE -DDEBUG -D__STM32__\
	-Wno-write-strings \
	-fno-exceptions -ffunction-sections -fdata-sections -fno-builtin \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float
	LDFLAGS+=-Tstm32_link.ld -nostartfiles -lm src/hal/libhal.a #nmea_parser/libnmea.a

else ifeq ($(TARGET), Linux)
	CFLAGS+=-O0 -g3 --std=c++0x -Wall -Iinclude -Inmea_parser -Iinclude/hal -D_GNU_SOURCE -DDEBUG
	LDFLAGS+=-lm
endif

OBJ=$(SRC:%.cc=%.o)

EXEC:=program

all: subdirs $(EXEC).elf

%.o: %.cc
	$(GPP) -c -o $@ $< $(CFLAGS)

$(EXEC).elf: $(OBJ)
	$(GPP) -o $@ $^ $(LDFLAGS)
	$(OCOPY) -O binary $(EXEC).elf $(EXEC).bin
	readelf -s program.elf | tail -n +2 | sort -nk3 > stats.txt

subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done


clean:
	$(DEL) -f $(EXEC).*
	find -type f -regex ".*/.*\.\(o\|d\\)" -delete
	for dir in $(SUBDIRS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir clean; \
	done

install:
	st-flash erase
	st-flash --reset write $(EXEC) 0x08000000

.PHONY: subdirs


