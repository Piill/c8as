#!/bin/bash

./build/c8as -i ./tests/input/test1.c8 -t ./tests/output/test1

TEST1=$( diff -i ./tests/output/test1 ./tests/control/test1 | wc -c )
if [ $TEST1 -eq 0 ]
then
	echo "Test 1 passed"
else
	echo "Test 1 failed"
fi

./build/c8as -i ./tests/input/test2_a.c8 -t ./tests/output/test2_a
./build/c8as -i ./tests/input/test2_b.c8 -t ./tests/output/test2_b

TEST2=$( diff -i ./tests/output/test2_a ./tests/output/test2_b | wc -c )
if [ $TEST2 -eq 0 ]
then
	echo "Test 2 passed"
else
	echo "Test 2 failed"
fi

