CC = g++
CFLAGS  = -g -Wall -std=c++11 -Wno-sign-compare -O3

default: run_plan

generated_query: generated_query.o relation.o yannakakis.o generic_join.o
	$(CC) $(CFLAGS) generated_query.o relation.o generic_join.o yannakakis.o -o generated_query.exe

triangle: example_triangle_query.o relation.o yannakakis.o generic_join.o
	$(CC) $(CFLAGS) example_triangle_query.o relation.o generic_join.o yannakakis.o -o triangle_handcoded.exe

relation_test: relation_test.o relation.o generic_join.o
	$(CC) $(CFLAGS) relation_test.o relation.o generic_join.o -o relation_test.exe

generic_join_test: generic_join_test.o relation.o generic_join.o
	$(CC) $(CFLAGS) generic_join_test.o relation.o generic_join.o -o generic_join_test.exe

y_test: relation.o generic_join.o yannakakis.o yannakakis_test.o
	$(CC) $(CFLAGS) relation.o generic_join.o yannakakis.o yannakakis_test.o -o yannakakis_test.exe

relation_test.o: test/relation_test.cc
	$(CC) $(CFLAGS) -c test/relation_test.cc

generic_join_test.o: test/generic_join_test.cc
	$(CC) $(CFLAGS) -c test/generic_join_test.cc

yannakakis_test.o: test/yannakakis_test.cc
	$(CC) $(CFLAGS) -c test/yannakakis_test.cc

pairwise: pairwise.o relation.o generic_join.o
	$(CC) $(CFLAGS) pairwise.o relation.o generic_join.o -o pairwise.exe

run_plan: run_plan.o relation.o  yannakakis.o generic_join.o
	$(CC) $(CFLAGS) run_plan.o relation.o generic_join.o yannakakis.o -o run_plan.exe

triangle.o: src/triangle.cc
	$(CC) $(CFLAGS) -c src/triangle.cc

pairwise.o: src/pairwise.cc
	$(CC) $(CFLAGS) -c src/pairwise.cc

run_plan.o: src/run_plan.cc
	$(CC) $(CFLAGS) -c src/run_plan.cc

generated_query.o: src/GENERATED_QUERY.cc
	$(CC) $(CFLAGS) -c src/GENERATED_QUERY.cc

example_triangle_query.o: src/example_triangle_query.cc
	$(CC) $(CFLAGS) -c src/example_triangle_query.cc

generic_join.o: src/generic_join.h src/generic_join.cc
	$(CC) $(CFLAGS) -c src/generic_join.cc

yannakakis.o: src/yannakakis.h src/yannakakis.cc
	$(CC) $(CFLAGS) -c src/yannakakis.cc

relation.o: src/relation.cc
	$(CC) $(CFLAGS) -c src/relation.cc

clean:
	$(RM) -r *.o *~ *.exe *.exe.dSYM
