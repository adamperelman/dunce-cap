CC = g++
CFLAGS  = -g -Wall -std=c++0x

default: main

test: relation_test.o relation.o
	$(CC) $(CFLAGS) relation_test.o relation.o -o runtest

relation_test.o: test/relation_test.cc
	$(CC) $(CFLAGS) -c test/relation_test.cc

main: main.o relation.o
	$(CC) $(CFLAGS) main.o relation.o -o main

main.o: src/main.cc
	$(CC) $(CFLAGS) -c src/main.cc

genericjoin:  database.o relation.o

test: database.o relation.o

database.o: src/database.h src/database.cc
	$(CC) $(CFLAGS) -c src/database.cc

relation.o: src/relation.cc
	$(CC) $(CFLAGS) -c src/relation.cc

clean:
	$(RM) *.o *~
