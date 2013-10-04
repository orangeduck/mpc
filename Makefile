CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror -Wno-unused -g

TESTS = $(wildcard tests/*.c)

all: check 
  
check: $(TESTS) mpc.c
	$(CC) $(CFLAGS) $^ -lm -o test
	./test
  
clean:
	rm test.exe
