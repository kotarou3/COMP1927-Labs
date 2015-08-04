include ../Makefile.inc

all: usel randl
clean:
	rm -f usel randl *.o

usel: useIntList.o IntList.o
	$(CC) -o $@ $+ $(LDFLAGS)

randl: randList.o
	$(CC) -o $@ $+ $(LDFLAGS)
