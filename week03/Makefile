include ../Makefile.inc

CFLAGS:= $(CFLAGS) -O3

all: testL myed
clean:
	rm -f testL myed *.o

testL: testList.o DLList.o
	$(CC) -o $@ $+ $(LDFLAGS)

myed: myed.o DLList.o
	$(CC) -o $@ $+ $(LDFLAGS)
