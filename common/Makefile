# Makefile for 'common' library
# Trung Nguyen, Spring 2023

OBJS = pagedir.o 
LIB = common.a 
L = ../libcs50


# uncomment the following to turn on verbose memory logging
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

$(LIB): $(OBJS)
	ar cr $(LIB) $(OBJS)

pagedir.o: pagedir.h

#test: pagedir
#	./pagedir

#valgrind: pagedir
#	$(VALGRIND) ./pagedir

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f core
	rm -f $(LIB)
