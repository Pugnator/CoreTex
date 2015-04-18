CC:=gcc
OCOPY:=arm-none-eabi-objcopy
STRIP:=arm-none-eabi-strip
DEL:=rm

SRC:=main.c

CFLAGS+=-O0 -g3 --std=gnu99 -Wall

EXEC:=znmea
OBJ:=$(SRC:.c=.o)

all: $(EXEC)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^

clean:
	$(DEL) -f $(EXEC).*
	find -type f -regex ".*/.*\.\(o\|d\\)" -delete

install:

debug:
