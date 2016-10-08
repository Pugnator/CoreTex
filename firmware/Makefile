MCU?=STM32F10X_MD
TARGET?=STM32


PROJECTNAME:=CoreTex
GCCPRFX:=arm-none-eabi

AR:=$(GCCPRFX)-ar
GPP:=$(GCCPRFX)-g++
ASM:=$(GCCPRFX)-as
OCOPY:=$(GCCPRFX)-objcopy
STRIP:=$(GCCPRFX)-strip
SIZE:=$(GCCPRFX)-size
MKDIR_P = mkdir -p
MAKE:=make --always-make

LIBSDIR:=libs
OBJSDIR:=obj
OUTPUTDIR:=bin
MRI:=usedlibs.mri

CORELIB:=\
	src/core

SUPPORT:=\
	src/support	

DRIVERS:=src/drivers

BLD?=DBG
export $(BLD)
ifeq ($(BLD), DBG)	
	CFLAGS:=-Og -g3 -DDEBUG	-ffunction-sections -fdata-sections
	LDFLAGS:=external/stdlib/Libraries/STM32F10x_StdPeriph_Driver/libstd.a -Wl,--gc-sections 
else
   CFLAGS:=-Os -ffunction-sections -fdata-sections
   LDFLAGS:=-Wl,--gc-sections 
endif

export CFLAGS
export LDFLAGS

SRC+=\
	src/main.cc	

CFLAGS+=\
	--std=gnu++11 -Wall \
	-Iinclude -Isrc \
	-Iexternal/stdlib/Libraries/STM32F10x_StdPeriph_Driver/inc \
	-Iexternal/stdlib/Libraries/CMSIS/CM3/DeviceSupport/ST/STM32F10x \
	-D$(MCU) -D_GNU_SOURCE -D__STM32__\
	-D_REENT_SMALL -D_REENT_GLOBAL_ATEXIT \
	-Wno-write-strings \
	-fno-exceptions \
	-fno-builtin -nostdlib \
	-mlittle-endian -mthumb -mcpu=cortex-m3 -msoft-float

LDFLAGS+=\
  -Tstm32_link.ld \
	-lm	
	
OBJ:=$(SRC:%.cc=%.o)

EXEC:=$(OUTPUTDIR)/$(PROJECTNAME).elf
LIBEXEC:=$(LIBSDIR)/$(PROJECTNAME).a

all: directories coretex $(EXEC)

coretex:	corelib drivers support $(LIBEXEC)

%.o: %.cc
	$(GPP) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(GPP) -o $@ $^ $(LDFLAGS) $(LIBEXEC)
	$(OCOPY) -O binary $(EXEC) $(EXEC).bin
	@stat -c "Output size of <%n> is %s" $(EXEC).bin
	@$(SIZE) --format=berkeley $(EXEC)


$(LIBEXEC): $(MRI)
	$(AR) M <$^
	@stat -c "Output size of <%n> is %s" $(LIBEXEC)

directories:	$(OUTPUTDIR)

$(OUTPUTDIR):
	$(MKDIR_P) $(OUTPUTDIR)

corelib:
	for dir in $(CORELIB); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done

support:
	for dir in $(SUPPORT); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done

drivers:
	for dir in $(DRIVERS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir; \
	done


clean:
	rm -f $(EXEC) $(EXEC).bin $(LIBEXEC)
	rm -rf $(OUTPUTDIR) $(LIBSDIR)	$(OBJSDIR)

	for dir in $(CORELIB); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir clean; \
	done

	for dir in $(DRIVERS); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir clean; \
	done

	for dir in $(SUPPORT); do \
		$(MAKE) TARGET=$(TARGET) -C $$dir clean; \
	done

.PHONY: corelib
.PHONY: drivers