SRC:=ff.c
CC:=arm-none-eabi-g++
OBJ=$(SRC:%.c=%.o)

LIBFAT32:=libfat32.a

CFLAGS+=-O2 -std=c++98 -I.

.PHONY: all
all: $(LIBFAT32)

%.o: %.c	
	$(CC) -c -o $@ $< $(CFLAGS)
	
$(LIBFAT32): $(OBJ)
	ar rcs $@ $^

.PHONY: clean
clean:
	/usr/bin/find . -name "*.o" -type f -print0 | xargs -0 /bin/rm -f
	/bin/rm -f $(LIBFAT32)
