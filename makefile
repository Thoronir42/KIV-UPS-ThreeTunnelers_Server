CFLAGS= -Wall -g -o
SRC=$(wildcard *.c)
foo: $(SRC)
	gcc -o $@ $^ $(CFLAGS)