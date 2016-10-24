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
MKDIR_P:=mkdir -p

PROJECTROOT:=../../../..
LIBSDIR:=../../../../libs
OBJSDIR:=../../../../obj
SRC:=$(wildcard *.cc)

MAKE:=make


CFLAGS+=\
	--std=gnu++11 -Wall \
	-I$(PROJECTROOT)/include -I$(PROJECTROOT)/src -I. \
	-Iexternal/stdlib/Libraries/STM32F10x_StdPeriph_Driver/inc \
	-Iexternal/stdlib/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
	-D$(MCU) -D_GNU_SOURCE -D__STM32__\
	-D_REENT_SMALL -D_REENT_GLOBAL_ATEXIT \
	-Wno-write-strings \
	-fno-exceptions \
	-fno-builtin -nostdlib \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

OBJ:=$(SRC:%.cc=$(OBJSDIR)/%.o)
LIBEXEC:=$(LIBSDIR)/$(LIBNAME).a

all: directories $(LIBEXEC)

directories:	$(LIBSDIR) $(OBJSDIR)

$(LIBSDIR):
	$(MKDIR_P) $(LIBSDIR)

$(OBJSDIR):
	$(MKDIR_P) $(OBJSDIR)

$(OBJSDIR)/%.o: %.cc
	$(GPP) -c -o $@ $< $(CFLAGS)

$(LIBEXEC): $(OBJ)
	$(AR) rcs $@ $^
	@stat -c "Output size of <%n> is %s" $(LIBEXEC)	

clean:
	rm -f $(LIBEXEC) $(OBJ)