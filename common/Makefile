# Makefile for 'hash' module
#
# David Kotz, April 2016, 2017, 2019, 2021
# updated by Xia Zhou, July 2016, 2018, January 2019

OBJS = pagedir.o word.o index.o
LIB = common.a 
L = ../libcs50
LLIBS = ../libcs50/libcs50.a


# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L
CC = gcc
MAKE = make
# for memory-leak tests
#VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

$(LIB): $(LLIBS) $(OBJS) 
	ar cr $@ $^

pagedir.o: pagedir.h 
index.o: index.h 
word.o: word.h

#.PHONY: test valgrind clean

#test: pagedir
#	./pagedir

#valgrind: pagedir
#	$(VALGRIND) ./pagedir

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core
	rm -f $(LIB)
