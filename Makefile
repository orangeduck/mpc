CC = gcc
CFLAGS = -ansi -Wall -Wpedantic -Werror -Wno-unused -g

TESTS = $(wildcard tests/*.c)

all: check 
  
check: $(TESTS) mpc.c
	$(CC) $(CFLAGS) $^ -o test
	./test
  
clean:
	rm test.exe