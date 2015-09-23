CC:=arm-none-eabi-gcc
OCOPY:=arm-none-eabi-objcopy
STRIP:=arm-none-eabi-strip
DEL:=rm

SRC:=config.cc  i2c.cc  isr.cc  rtc.cc  spi.cc  startup_stm32f10x_md.s  timers.cc  usart.cc startup_stm32f10x_md.s

CFLAGS+=\
	-O0 -g3 --std=gnu++11 -Wall \
	-I../../include -DSTM32F10X_MD \
	-D_GNU_SOURCE -DDEBUG -D__STM32__\
	-Wno-write-strings \
	-fno-exceptions -ffunction-sections -fdata-sections -fno-builtin \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

EXEC:=libhal.a
OBJ=$(join $(addsuffix stm32/, $(dir $(SRC))), $(notdir $(SRC:.cc=.o)))

all: $(EXEC)

%.o: %.cc
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	arm-none-eabi-ar rcs $@ $^

clean:
	$(DEL) -f $(EXEC)
	find -type f -regex ".*/.*\.\(o\|d\\)" -delete

install:

debug:
