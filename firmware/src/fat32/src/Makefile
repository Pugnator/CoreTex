SRC:=ff.c
CC:=arm-none-eabi-g++
OBJ=$(SRC:%.c=%.o)

LIBFAT32:=../lib/libfat32.a

CFLAGS+=-O2 -std=c++98 -I.

.PHONY: all
all: $(LIBFAT32)

%.o: %.c	
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(LIBFAT32): $(OBJ)
	ar rcs $(LIBFAT32) $@ $^

.PHONY: clean
clean:
	@find -type f -regex ".*/.*\.\(o\|a\|out\\)" -delete
	@-rm -f $(LIBFAT32)
