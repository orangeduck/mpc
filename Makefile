CC ?= gcc
AR ?= ar

prefix ?= /usr/local
libdir ?= $(prefix)/lib
include?= $(prefix)/include

STND=-ansi
CFLAGS = $(STND) -pedantic -O3 -g -Wall -Werror -Wextra -Wformat=2 -Wshadow \
  -Wno-long-long -Wno-overlength-strings -Wno-format-nonliteral -Wcast-align \
  -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls \
  -Wnested-externs -Wmissing-include-dirs -Wswitch-default

ifeq "$(shell uname -s)" "Darwin"
	SHARED_EXT=dylib
else
	SHARED_EXT=so
endif

TESTS = $(wildcard tests/*.c)
EXAMPLES = $(wildcard examples/*.c)
EXAMPLESEXE = $(EXAMPLES:.c=)

mpc.o: mpc.c mpc.h
	$(CC) $(CFLAGS) -c mpc.c -o mpc.o

examples/%: examples/%.c mpc.c
	$(CC) $(CFLAGS) $^ -lm -o $@

.PHONY: all check install static shared libs clean

all: $(EXAMPLESEXE) check libs

static: mpc.o
	$(AR) rcs libmpc.a mpc.o

shared: mpc.o
	$(CC) mpc.o -shared -o libmpc.$(SHARED_EXT)

libs: static shared

install: check libs
	mkdir -p $(prefix)
	mkdir -p $(libdir)
	mkdir -p $(include)
	cp mpc.h $(include)
	cp libmpc.a libmpc.$(SHARED_EXT) $(libdir)

check: $(TESTS) mpc.c
	$(CC) $(filter-out -Werror, $(CFLAGS)) $^ -lm -o test
	./test

clean:
	rm -rf test examples/doge examples/lispy examples/maths examples/smallc \
	examples/foobar examples/tree_traversal
