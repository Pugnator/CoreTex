CC:=arm-none-eabi-gcc
AS:=arm-none-eabi-gcc
OCOPY:=arm-none-eabi-objcopy
STRIP:=arm-none-eabi-strip
COPY:=cp
DEL:=rm

STARTUP:=startup_stm32f10x_md.s

SRC=main.c usart.c config.c \
$(STARTUP)

CFLAGS+=-O0 -g3 -std=gnu99 -Wall -I. -DSTM32F10X_MD -D_GNU_SOURCE \
-fno-exceptions -ffunction-sections -fdata-sections -fno-builtin \
-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float 

LDFLAGS+=-Tstm32_link.ld -nostartfiles 

OBJ=$(SRC:%.c=%.o)
EXEC:=program

all: $(EXEC).elf
	
%.o: %.c	
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC).elf: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)		
	$(OCOPY) -O binary $(EXEC).elf $(EXEC).bin
	

clean:
	rm -f $(EXEC).*
	find -type f -regex ".*/.*\.\(o\|d\\)" -delete
