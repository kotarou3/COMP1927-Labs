include ../Makefile.inc

CFLAGS += -D_GNU_SOURCE
LDFLAGS += -lcurl

.PHONY: test

all: crawl test
clean:
	rm -f crawl ts tq tk tg graph.json *.o *.out

test: crawl ts tq tk tg
	./ts > ts.out
	./tq > tq.out
	./tk > tk.out
	./tg > tg.out
	./crawl "http://www.cse.unsw.edu.au/~cs1927/15s2/mini-web/" 100 > graph.json

crawl: crawl.o url_file.o graph.o queue.o set.o stack.o
	$(CC) -o $@ $+ $(LDFLAGS)

ts : ts.o set.o
	$(CC) -o $@ $+ $(LDFLAGS)
tq : tq.o queue.o
	$(CC) -o $@ $+ $(LDFLAGS)
tk : tk.o stack.o
	$(CC) -o $@ $+ $(LDFLAGS)
tg : tg.o graph.o
	$(CC) -o $@ $+ $(LDFLAGS)
