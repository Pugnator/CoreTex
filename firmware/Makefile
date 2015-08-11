GCC:=arm-none-eabi-g++
OCOPY:=arm-none-eabi-objcopy
STRIP:=arm-none-eabi-strip
DEL:=rm

STARTUP:=startup_stm32f10x_md.s

SRC:=\
	main.cc \
	usart.cc \
	config.cc \
	irq.cc \
	compat.cc \
	log.cc \
	spi.cc \
	i2c.cc \
	timers.cc \
	rtc.cc \
	gps.cc \
	gsm.cc \
	sdcard.cc \
	common.cc \
	morse.cc \
	$(STARTUP)

CFLAGS+=-O0 -g3 --std=c++11 -Wall -Iinclude -Isrc/fat32/src -DSTM32F10X_MD -D_GNU_SOURCE \
-fno-exceptions -ffunction-sections -fdata-sections -fno-builtin \
-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

LDFLAGS+=-Tstm32_link.ld -nostartfiles src/fat32/lib/libfat32.a -lm

OBJ=$(join $(addsuffix src/, $(dir $(SRC))), $(notdir $(SRC:.cc=.o)))

EXEC:=bin/program

all: $(EXEC).elf

%.o: %.cc
	$(GCC) -c -o $@ $< $(CFLAGS)

$(EXEC).elf: $(OBJ)
	$(GCC) -o $@ $^ $(LDFLAGS)
	$(OCOPY) -O binary $(EXEC).elf $(EXEC).bin


clean:
	$(DEL) -f $(EXEC).*
	find -type f -regex ".*/.*\.\(o\|d\\)" -delete

install:

debug:
