include ../Makefile.inc
SHELL=bash

.PHONY: test

all: bst mkrand mkpref test
clean:
	rm -f bst mkrand mkpref *.o

test: bst $(wildcard tests/tree[0-6])
	for TEST in tests/tree[0-6]; do \
		diff -u "$$TEST".expected <(./bst < "$$TEST") || exit 1; \
	done

bst: bst.o BSTree.o Queue.o
	$(CC) -o $@ $+ $(LDFLAGS)

mkrand: mkrand.c
	$(CC) -o $@ $+ $(LDFLAGS)
mkpref: mkpref.c
	$(CC) -o $@ $+ $(LDFLAGS)
