CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wno-overlength-strings -Werror -g

TESTS = $(wildcard tests/*.c)
EXAMPLES = $(wildcard examples/*.c)
EXAMPLESEXE = $(EXAMPLES:.c=)

all: $(EXAMPLESEXE) check 
  
check: $(TESTS) mpc.c
	$(CC) $(CFLAGS) $^ -lm -o test
	./test
  
examples/%: examples/%.c mpc.c
	$(CC) $(CFLAGS) $^ -lm -o $@
  
clean:
	rm test
