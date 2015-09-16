include ../Makefile.inc

.PHONY: test

all: pl euro conn test
clean:
	rm -f pl euro conn *.o

test: conn conn-tester.js
	./conn-tester.js

pl: pl.o Places.o
	$(CC) -o $@ $+ $(LDFLAGS)

euro: euro.o Map.o Places.o
	$(CC) -o $@ $+ $(LDFLAGS)

conn: conn.o Map.o Places.o
	$(CC) -o $@ $+ $(LDFLAGS)
