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
SRC:=$(wildcard *.c)

MAKE:=make

CFLAGS+=-Os -ffunction-sections -fdata-sections -std=c++98 -I.

OBJ:=$(SRC:%.c=$(OBJSDIR)/%.o)
LIBEXEC:=$(LIBSDIR)/$(LIBNAME).a

all: directories $(LIBEXEC)

directories:	$(LIBSDIR) $(OBJSDIR)

$(LIBSDIR):
	$(MKDIR_P) $(LIBSDIR)

$(OBJSDIR):
	$(MKDIR_P) $(OBJSDIR)

$(OBJSDIR)/%.o: %.c
	$(GPP) -c -o $@ $< $(CFLAGS)

$(LIBEXEC): $(OBJ)
	$(AR) rcs $@ $^
	@stat -c "Output size of <%n> is %s" $(LIBEXEC)

clean:
	rm -f $(LIBEXEC) $(OBJ)