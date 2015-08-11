CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
LDFLAGS =

.PHONY: all clean

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)
