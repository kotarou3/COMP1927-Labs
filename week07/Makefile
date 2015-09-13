include ../Makefile.inc

all: pl euro conn
clean:
	rm -f testL myed *.o

pl: pl.o Places.o
	$(CC) -o $@ $+ $(LDFLAGS)

euro: euro.o Map.o Places.o
	$(CC) -o $@ $+ $(LDFLAGS)

conn: conn.o Map.o Places.o
	$(CC) -o $@ $+ $(LDFLAGS)
