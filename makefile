CC      = gcc
CFLAGS  = -g -O2 -Wall
LFLAGS  = -lncurses

PROGS= mine

# Default Rules:
.c:
	$(CC) $(CFLAGS) $(LFLAGS) $< -o $@

all: $(PROGS)

mine:  mine.h screen.c libmine.c
	$(CC) $@.c mine.h screen.c libmine.c -o $@ $(CFLAGS) $(LFLAGS)
	rm -f $@.o

install: all

clean:
	rm -f $(PROGS) *.o

