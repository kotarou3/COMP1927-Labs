include ../Makefile.inc

CFLAGS += -D_BSD_SOURCE -pg -msse4.2
LDFLAGS += -pg

all: words mkwords
clean:
	rm -f words mkwords *.o gmon.out

words: words.o HashTable.o List.o Item.o
	$(CC) -o $@ $+ $(LDFLAGS)
mkwords: mkwords.o
	$(CC) -o $@ $+ $(LDFLAGS)
