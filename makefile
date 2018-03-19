CFLAGS= -g -Wall -Wpedantic -Wextra
CFILES=$(wildcard ./scrs/*.c)


all:
	gcc $(CFILES) -o ./build/c8as $(CFLAGS)

test: all
	./build/c8as -i ./tests/input/test1.c8 -t ./tests/output/test1
	diff -i ./tests/output/test1 ./tests/control/test1 -y

clean: 
	rm -r ./build/*
