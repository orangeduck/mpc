PROJ = mpc
CC ?= gcc
STND = -ansi
DIST = build
MKDIR  ?= mkdir -p
PREFIX ?= /usr/local
CFLAGS ?= $(STND) -pedantic -O3 -g -Wall -Werror -Wextra -Wformat=2 -Wshadow \
  -Wno-long-long -Wno-overlength-strings -Wno-format-nonliteral -Wcast-align \
  -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls \
  -Wnested-externs -Wmissing-include-dirs -Wswitch-default

TESTS = $(wildcard tests/*.c)
EXAMPLES = $(wildcard examples/*.c)
EXAMPLESEXE = $(EXAMPLES:.c=)

.PHONY: all check clean libs

all: $(EXAMPLESEXE) check

$(DIST):
	$(MKDIR) $(DIST)/examples

check: test-file test-static test-dynamic
	./$(DIST)/test-file
	./$(DIST)/test-static
	LD_LIBRARY_PATH=$(DIST) ./$(DIST)/test-dynamic

test-file: $(DIST) $(TESTS) $(PROJ).c mpc.h tests/ptest.h
	$(CC) $(filter-out -Werror, $(CFLAGS)) $(TESTS) $(PROJ).c -lm -o $(DIST)/test-file

test-dynamic: $(DIST) $(TESTS) lib$(PROJ).so mpc.h tests/ptest.h
	$(CC) $(filter-out -Werror, $(CFLAGS)) $(TESTS) -lm -L$(DIST) -l$(PROJ) -o $(DIST)/test-dynamic

test-static: $(DIST) $(TESTS) lib$(PROJ).a mpc.h tests/ptest.h
	$(CC) $(filter-out -Werror, $(CFLAGS)) $(TESTS) -lm -L$(DIST) -l$(PROJ) -static -o $(DIST)/test-static

examples/%: $(DIST) examples/%.c $(PROJ).c mpc.h
	$(CC) $(CFLAGS) $(filter-out $(DIST), $^) -lm -o $(DIST)/$@

lib$(PROJ).so: $(DIST) $(PROJ).c mpc.h
ifneq ($(OS),Windows_NT)
	$(CC) $(CFLAGS) -fPIC -shared $(PROJ).c -o $(DIST)/lib$(PROJ).so
else
	$(CC) $(CFLAGS) -shared $(PROJ).c -o $(DIST)/lib$(PROJ).so
endif

lib$(PROJ).a: $(DIST) $(PROJ).c mpc.h
	$(CC) $(CFLAGS) -c $(PROJ).c -o $(DIST)/$(PROJ).o
	$(AR) rcs $(DIST)/lib$(PROJ).a $(DIST)/$(PROJ).o

libs: lib$(PROJ).so lib$(PROJ).a
  
clean:
	rm -rf -- $(DIST)

install: all
	install -d -m644 $(DESTDIR)$(PREFIX)/{include,lib,share/$(PROJ)}
	install -m755 -t $(DESTDIR)$(PREFIX)/lib $(DIST)/lib*
	install -m644 -t $(DESTDIR)$(PREFIX)/share/$(PROJ) $(PROJ).{c,h}
	install -m644 $(PROJ).h $(DESTDIR)$(PREFIX)/include/$(PROJ).h

uninstall:
	rm -rf -- \
		$(DESTDIR)$(PREFIX)/include/$(PROJ).h \
		$(DESTDIR)$(PREFIX)/share/$(PROJ)/$(PROJ).{c,h} \
		$(DESTDIR)$(PREFIX)/lib/lib$(PROJ).{so,a}
