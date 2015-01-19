CC = gcc
CFLAGS  = -g -Wall -std=c++0x

default: genericjoin

genericjoin:  database.o relation.o 

database.o: database.h database.cc
	$(CC) $(CFLAGS) -c database.cc

relation.o: relation.h relation.cc
	$(CC) $(CFLAGS) -c relation.cc

clean: 
	$(RM) count *.o *~
