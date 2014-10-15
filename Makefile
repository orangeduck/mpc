# change notes:
# 	filter out -Werror when compiling ptest. GCC chooses to warn about strange things sometimes.
# 	mpc.c and mpc.h ompile will all flags below on clang and gcc with -Werror.

CC = gcc
CFLAGS = -std=c11 -O3 -g -Werror -Wall -Wextra -Wformat=2 -Wshadow \
		 -Wno-format-nonliteral -Wcast-align -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
		 -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wswitch-default

TESTS = $(wildcard tests/*.c)
EXAMPLES = $(wildcard examples/*.c)
EXAMPLESEXE = $(EXAMPLES:.c=)

all: $(EXAMPLESEXE) check 

check: $(TESTS) mpc.c
	$(CC) $(filter-out -Werror, $(CFLAGS)) $^ -lm -o test
	./test

examples/%: examples/%.c mpc.c
	$(CC) $(CFLAGS) $^ -lm -o $@
  
clean:
	rm -rf test examples/doge examples/lispy examples/maths examples/smallc
