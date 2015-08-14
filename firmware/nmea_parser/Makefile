CC:=arm-none-eabi-gcc
OCOPY:=arm-none-eabi-objcopy
STRIP:=arm-none-eabi-strip
DEL:=rm

SRC:=main.c gga.c vtg.c rmc.c

CFLAGS+=-O0 -g3 --std=gnu99 -Wall -I.

EXEC:=libnmea.a
OBJ:=$(SRC:.c=.o)

all: $(EXEC)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	arm-none-eabi-ar rcs $@ $^
	#$(CC) -o $@ $^

clean:
	$(DEL) -f $(EXEC)
	find -type f -regex ".*/.*\.\(o\|d\\)" -delete

install:

debug:
