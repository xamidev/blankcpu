CC=gcc
CFLAGS=-g -Wall -Wextra

all: asm
	$(CC) *.c $(CFLAGS) -o dumb8

asm:
	$(CC) assembler/*.c $(CFLAGS) -o das

clean:
	rm dumb8 das 
