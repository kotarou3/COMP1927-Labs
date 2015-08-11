include ../Makefile.inc

CFLAGS := $(CFLAGS) -O3

all: vlad
clean:
	rm -f vlad *.o

vlad: vlad.o allocator.o
	$(CC) -o $@ $+ $(LDFLAGS)
