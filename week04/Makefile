include ../Makefile.inc

CFLAGS:= $(CFLAGS) -D_GNU_SOURCE -O3

all: sorter testQ
clean:
	rm -f testQ sorter *.o

sorter: sorter.o
	$(CC) -o $@ $+ $(LDFLAGS)

testQ: testQ.o Queue.o
	$(CC) -o $@ $+ $(LDFLAGS)
