DESCRIPTION:=driver.info
MCU?=STM32F10X_MD
LIBNAME:=$(shell sed -n '/^name=/ {s///p;q;}' $(DESCRIPTION))
GCCPRFX:=arm-none-eabi

AR:=$(GCCPRFX)-ar
GPP:=$(GCCPRFX)-g++
ASM:=$(GCCPRFX)-as
OCOPY:=$(GCCPRFX)-objcopy
STRIP:=$(GCCPRFX)-strip
SIZE:=$(GCCPRFX)-size
RM:=rm -f
MKDIR_P:=mkdir -p


PROJECTROOT:=../..
LIBSDIR:=../../libs
OBJSDIR:=../../obj

CFLAGS+=\
	--std=gnu++11 -Wall \
	-I$(PROJECTROOT)/include -I$(PROJECTROOT)/src \
	-Iexternal/stdlib/Libraries/STM32F10x_StdPeriph_Driver/inc \
	-Iexternal/stdlib/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
	-D$(MCU) -D_GNU_SOURCE -D__STM32__\
	-D_REENT_SMALL -D_REENT_GLOBAL_ATEXIT \
	-Wno-write-strings \
	-fno-exceptions \
	-fno-builtin -nostdlib \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

SRC:=$(wildcard *.cc)

CFLAGS+=\
	--std=gnu++11 -Wall \
	-I../../include -DSTM32F10X_MD \
	-D_GNU_SOURCE -DDEBUG -D__STM32__\
	-Wno-write-strings \
	-fno-exceptions -fno-builtin \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

LIBEXEC:=$(LIBSDIR)/$(LIBNAME).a
OBJ:=$(SRC:%.cc=$(OBJSDIR)/%.o)

all: directories $(LIBEXEC)

directories:	$(LIBSDIR) $(OBJSDIR)

$(LIBSDIR):
	$(MKDIR_P) $(LIBSDIR)

$(OBJSDIR):
	$(MKDIR_P) $(OBJSDIR)

$(OBJSDIR)/%.o: %.cc
	$(GPP) -c -o $@ $< $(CFLAGS)

$(LIBEXEC): $(OBJ) $(STARTUP)
	$(AR) rcs $@ $^

clean:
	$(RM) $(LIBEXEC) $(OBJ)

