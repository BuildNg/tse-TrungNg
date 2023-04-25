# Makefile for the 'crawler' program
# Trung Nguyen, Spring 2023

L = ../libcs50
C = ../common

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$C
OBJS = crawler.o
LIBS = 
LLIBS = $L/libcs50.a $C/common.a

CC = gcc
MAKE = make
# memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: test valgrind clean all

all = crawler

crawler: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

test: crawler
	bash -v testing.sh

valgrind: crawler
	$(VALGRIND) bash -v testing.sh
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f testing.out
	rm -f core
