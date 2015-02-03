CC = clang++
CFLAGS  = -g -Wall -std=c++0x -O0

default: triangle

relation_test: relation_test.o relation.o
	$(CC) $(CFLAGS) relation_test.o relation.o -o relation_test.exe

generic_join_test: generic_join_test.o relation.o database.o
	$(CC) $(CFLAGS) generic_join_test.o relation.o database.o -o generic_join_test.exe

relation_test.o: test/relation_test.cc
	$(CC) $(CFLAGS) -c test/relation_test.cc

generic_join_test.o: test/generic_join_test.cc
	$(CC) $(CFLAGS) -c test/generic_join_test.cc

triangle: triangle.o relation.o database.o
	$(CC) $(CFLAGS) triangle.o relation.o database.o -o triangle.exe

triangle.o: src/triangle.cc
	$(CC) $(CFLAGS) -c src/triangle.cc

database.o: src/database.h src/database.cc
	$(CC) $(CFLAGS) -c src/database.cc

relation.o: src/relation.cc
	$(CC) $(CFLAGS) -c src/relation.cc

clean:
	$(RM) *.o *~
