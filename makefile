CFLAGS= -g -Wall -Wpedantic -Wextra
CFILES=$(wildcard ./scrs/*.c)


all:
	gcc $(CFILES) -o ./build/c8as $(CFLAGS)

test: all
	./run_tests.sh

clean:
	rm -r ./build/*
