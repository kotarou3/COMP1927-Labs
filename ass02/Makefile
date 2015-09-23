CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic

.PHONY: all clean

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS)

all: testGameView testHunterView testDracView
clean:
	rm -f testGameView testHunterView testDracView *.o

testGameView: testGameView.o GameView.o Map.o Places.o
	$(CC) -o $@ $+
testHunterView: testHunterView.o HunterView.o Map.o Places.o
	$(CC) -o $@ $+
testDracView: testDracView.o DracView.o Map.o Places.o
	$(CC) -o $@ $+
