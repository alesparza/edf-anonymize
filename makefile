CC = gcc

.PHONY: all clean

default: all

all: edf-anonymizer

mini-hexdump.o: mini-hexdump.c
	$(CC) -g -Wall -c mini-hexdump.c -o mini-hexdump.o

edf-anonymizer: edf-anonymizer.c mini-hexdump.o
	$(CC) -g -Wall edf-anonymizer.c mini-hexdump.o -o edf-anonymizer

clean: 
	rm -f edf-anonymizer *.o
