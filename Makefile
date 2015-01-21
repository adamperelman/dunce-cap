CC = g++
CFLAGS  = -g -Wall -std=c++0x

default: main

relation_test: relation_test.o relation.o
	$(CC) $(CFLAGS) relation_test.o relation.o -o relation_test.exe

generic_join_test: generic_join_test.o relation.o generic_join.o
	$(CC) $(CFLAGS) generic_join_test.o relation.o generic_join.o -o generic_join_test.exe

relation_test.o: test/relation_test.cc
	$(CC) $(CFLAGS) -c test/relation_test.cc

generic_join_test.o: test/relation_test.cc
	$(CC) $(CFLAGS) -c test/generic_join_test.cc

main: main.o relation.o
	$(CC) $(CFLAGS) main.o relation.o -o main.exe

main.o: src/main.cc
	$(CC) $(CFLAGS) -c src/main.cc

generic_join.o: src/generic_join.h src/generic_join.cc
	$(CC) $(CFLAGS) -c src/generic_join.cc

database.o: src/database.h src/database.cc
	$(CC) $(CFLAGS) -c src/database.cc

relation.o: src/relation.cc
	$(CC) $(CFLAGS) -c src/relation.cc

clean:
	$(RM) *.o *~
