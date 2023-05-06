# Makefile for the 'indexer' program
# Trung Nguyen, Spring 2023

L = ../libcs50
C = ../common

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$C -I$L
OBJS = querier.o fuzzquery.o
LIBS = 
LLIBS = ../common/common.a ../libcs50/libcs50.a 

CC = gcc
MAKE = make
# memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: test valgrind clean all

all: querier fuzzquery

querier: querier.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

fuzzquery: fuzzquery.o $(LLIBS)
	$(CC) $(CFLAGS) $^ -o $@

test: querier fuzzquery
	bash -v testing.sh

valgrind: querier fuzzquery
	$(VALGRIND) ./querier ../data/toscrape-2 ../data/toscrape2.index < testfiles/valgrindtest
	$(VALGRIND) ./fuzzquerry ../data/toscrape2.index 10 141 | ./querier ../data/toscrape-2 ../data/toscrape2.index

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
#	rm -f testing.out
	rm -f core
	rm -f querier
	rm -f fuzzquery
