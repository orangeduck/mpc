CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror -g

TESTS = $(wildcard tests/*.c)

all: check 
  
check: $(TESTS) mpc.c
	$(CC) $(CFLAGS) $^ -lm -o test
	./test
  
clean:
	rm test
