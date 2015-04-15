CC:=arm-none-eabi-g++
AS:=arm-none-eabi-g++
OCOPY:=arm-none-eabi-objcopy
STRIP:=arm-none-eabi-strip
COPY:=cp
DEL:=rm

STARTUP:=startup_stm32f10x_md.s

SRC=main.cc usart.cc config.cc irq.cc \
$(STARTUP)

CFLAGS+=-O0 -g3 --std=c++98 -Wall -I. -DSTM32F10X_MD -D_GNU_SOURCE \
-fno-exceptions -ffunction-sections -fdata-sections -fno-builtin \
-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

LDFLAGS+=-Tstm32_link.ld -nostartfiles

OBJ=$(SRC:%.cc=%.o)
EXEC:=program

all: $(EXEC).elf

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC).elf: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	$(OCOPY) -O binary $(EXEC).elf $(EXEC).bin


clean:
	rm -f $(EXEC).*
	find -type f -regex ".*/.*\.\(o\|d\\)" -delete
