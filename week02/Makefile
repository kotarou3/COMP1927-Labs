include ../Makefile.inc

CFLAGS := $(CFLAGS) -O3

.PHONY: build

all: usel randl
build: timing.txt
clean:
	rm -f usel randl timing.txt *.o

usel: useIntList.o IntList.o
	$(CC) -o $@ $+ $(LDFLAGS)

randl: randList.o
	$(CC) -o $@ $+ $(LDFLAGS)

timing.txt: usel randl timing.sh
	./timing.sh | tee timing.txt
