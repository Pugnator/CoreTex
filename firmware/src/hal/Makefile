GCCPRFX:=arm-none-eabi

AR:=$(GCCPRFX)-ar
GPP:=$(GCCPRFX)-g++
ASM:=$(GCCPRFX)-as
OCOPY:=$(GCCPRFX)-objcopy
STRIP:=$(GCCPRFX)-strip
SIZE:=$(GCCPRFX)-size
DEL:=rm

BLD?=DBG
ifeq ($(BLD), DBG)	
	CFLAGS:=-Og -g3
else ifeq ($(BLD), REL)
    CFLAGS:=-Os -ffunction-sections -fdata-sections
    LDFLAGS+=-Wl,--gc-sections 
endif

SRC:=\
	core.cc \
	adc.cc \
	i2c.cc \
	isr.cc \
	rtc.cc \
	spi.cc \
	vmmu.cc \
	timers.cc \
	usart.cc \
	isr_helper.cc

CFLAGS+=\
	--std=gnu++11 -Wall \
	-I../../include -DSTM32F10X_MD \
	-D_GNU_SOURCE -DDEBUG -D__STM32__\
	-Wno-write-strings \
	-fno-exceptions -fno-builtin \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

EXEC:=libhal.a
OBJ=$(SRC:.cc=.o)

all: $(EXEC)

%.o: %.cc
	$(GPP) -c -o $@ $< $(CFLAGS)

startup.o: startup.s
	$(ASM) startup.s -o startup.o	

$(EXEC): $(OBJ) startup.o
	$(AR) rcs $@ $^

clean:
	/bin/rm -f $(EXEC)
	/usr/bin/find . -name "*.o" -type f -print0 | xargs -0 /bin/rm -f

install:

debug:
