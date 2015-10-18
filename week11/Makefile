include ../Makefile.inc
SHELL = bash

.PHONY: test

all: tlab test
clean:
	rm -f tlab *.o

test: runtests
	diff -u expected <(./runtests)

tlab: tlab.o Tree.o
	$(CC) -o $@ $+ $(LDFLAGS)
