CC = g++
CFLAGS  = -g -Wall -std=c++0x

default: main

main: main.o relation.o
	$(CC) $(CFLAGS) main.o relation.o -o main

main.o: main.cc
	$(CC) $(CFLAGS) main.cc

genericjoin:  database.o relation.o

test: database.o relation.o

database.o: database.h database.cc
	$(CC) $(CFLAGS) -c database.cc

relation.o: relation.h relation.cc
	$(CC) $(CFLAGS) -c relation.cc

clean:
	$(RM) *.o *~
