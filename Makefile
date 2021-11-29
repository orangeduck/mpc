PROJ = mpc
CC ?= gcc
STD ?= -ansi
DIST = build
MKDIR  ?= mkdir -p
PREFIX ?= /usr/local
CFLAGS ?= $(STD) -pedantic -O3 -g -Wall -Werror -Wextra -Wformat=2 -Wshadow \
  -Wno-long-long -Wno-overlength-strings -Wno-format-nonliteral -Wcast-align \
  -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls \
  -Wnested-externs -Wmissing-include-dirs -Wswitch-default

TESTS = $(wildcard tests/*.c)
EXAMPLES = $(wildcard examples/*.c)
EXAMPLESEXE = $(EXAMPLES:.c=)

.PHONY: all check clean libs $(DIST)/$(PROJ).pc

all: $(EXAMPLESEXE) check

$(DIST):
	$(MKDIR) $(DIST)
	$(MKDIR) $(DIST)/examples

check: $(DIST) $(DIST)/test-file $(DIST)/test-static $(DIST)/test-dynamic
	./$(DIST)/test-file
	./$(DIST)/test-static
	LD_LIBRARY_PATH=$(DIST) ./$(DIST)/test-dynamic

$(DIST)/test-file: $(TESTS) $(PROJ).c $(PROJ).h tests/ptest.h
	$(CC) $(filter-out -Werror, $(CFLAGS)) $(TESTS) $(PROJ).c -lm -o $(DIST)/test-file

$(DIST)/test-dynamic: $(TESTS) $(DIST)/lib$(PROJ).so $(PROJ).h tests/ptest.h
	$(CC) $(filter-out -Werror, $(CFLAGS)) $(TESTS) -lm -L$(DIST) -l$(PROJ) -o $(DIST)/test-dynamic

$(DIST)/test-static: $(TESTS) $(DIST)/lib$(PROJ).a $(PROJ).h tests/ptest.h
	$(CC) $(filter-out -Werror, $(CFLAGS)) $(TESTS) -lm -L$(DIST) -l$(PROJ) -static -o $(DIST)/test-static

examples/%: $(DIST) examples/%.c $(PROJ).c $(PROJ).h
	$(CC) $(CFLAGS) $(filter-out $(DIST) $(PROJ).h, $^) -lm -o $(DIST)/$@

$(DIST)/lib$(PROJ).so: $(DIST) $(PROJ).c $(PROJ).h
ifneq ($(OS),Windows_NT)
	$(CC) $(CFLAGS) -fPIC -shared $(PROJ).c -o $(DIST)/lib$(PROJ).so
else
	$(CC) $(CFLAGS) -shared $(PROJ).c -o $(DIST)/lib$(PROJ).so
endif

$(DIST)/lib$(PROJ).a: $(DIST) $(PROJ).c $(PROJ).h
	$(CC) $(CFLAGS) -c $(PROJ).c -o $(DIST)/$(PROJ).o
	$(AR) rcs $(DIST)/lib$(PROJ).a $(DIST)/$(PROJ).o

libs: $(DIST)/lib$(PROJ).so $(DIST)/lib$(PROJ).a

$(DIST)/$(PROJ).pc: $(DIST) $(PROJ).pc
	cp $(PROJ).pc $(DIST)/$(PROJ).pc
	sed -i '1i\prefix=$(PREFIX)/' $(DIST)/$(PROJ).pc

clean:
	rm -rf -- $(DIST)

install: all
	install -d -m644 $(DESTDIR)$(PREFIX)/include
	install -d -m644 $(DESTDIR)$(PREFIX)/lib/pkgconfig
	install -d -m644 $(DESTDIR)$(PREFIX)/share/$(PROJ)
	install -m755 -t $(DESTDIR)$(PREFIX)/lib $(DIST)/lib*
	install -m644 -t $(DESTDIR)$(PREFIX)/share/$(PROJ) $(PROJ).c $(PROJ).h
	install -m644 $(PROJ).h $(DESTDIR)$(PREFIX)/include/$(PROJ).h
	install -m644 $(PROJ).pc $(DESTDIR)$(PREFIX)/lib/pkgconfig/$(PROJ).pc

uninstall:
	rm -rf -- \
		$(DESTDIR)$(PREFIX)/include/$(PROJ).h \
		$(DESTDIR)$(PREFIX)/share/$(PROJ)/$(PROJ).{c,h} \
		$(DESTDIR)$(PREFIX)/lib/lib$(PROJ).{so,a}
