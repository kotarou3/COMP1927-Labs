include ../Makefile.inc

CFLAGS += -D_BSD_SOURCE
LDFLAGS += -lcurl

.PHONY: test

all: crawl test
clean:
	rm -f crawl tu ts tq tk tg *.o *.out

test: crawl ts tq tk tg tu
	./ts > ts.out
	./tq > tq.out
	./tk > tk.out
	./tg > tg.out
	./tu "http://www.cse.unsw.edu.au/~cs1927/15s2/mini-web/" > tu.out

crawl: crawl.o url_file.o url.o set.o queue.o graph.o stack.o
	$(CC) -o $@ $+ $(LDFLAGS)

ts : ts.o set.o
	$(CC) -o $@ $+ $(LDFLAGS)
tq : tq.o queue.o
	$(CC) -o $@ $+ $(LDFLAGS)
tk : tk.o stack.o
	$(CC) -o $@ $+ $(LDFLAGS)
tg : tg.o graph.o
	$(CC) -o $@ $+ $(LDFLAGS)
tu: tu.o url.o url_file.o
	$(CC) -o $@ $+ $(LDFLAGS)
