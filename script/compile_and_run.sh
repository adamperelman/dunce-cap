#!/bin/sh

# Compiles and runs the scala code, then compiles and runs
# the resulting query.

echo "compiling scala classes"
scalac -d classes query_compiler/src/QueryCompiler.scala

echo "running scala code"
scala -classpath classes QueryCompiler src/GENERATED_QUERY.cc

echo "making generated C++ code"
make generated_query

echo "running query"
./generated_query.exe
