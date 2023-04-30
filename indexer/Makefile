# Makefile for the 'indexer' program
# Trung Nguyen, Spring 2023

L = ../libcs50
C = ../common

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$C -I$L
OBJS = indexer.o indextest.o
LIBS = 
LLIBS = ../common/common.a ../libcs50/libcs50.a 

CC = gcc
MAKE = make
# memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: test valgrind clean all

all: indexer indextest

indexer: indexer.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

indextest: indextest.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

test: indexer indextest
	bash -v testing.sh

valgrind: indexer indextest
	$(VALGRIND) ./indexer ../data/toscrape-1 ../data/toscrape1.index
	$(VALGRIND) ./indextest ../data/toscrape1.index ../data/toscrape1.test

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
#	rm -f testing.out
	rm -f core
	rm -f indexer
	rm -f indextest
