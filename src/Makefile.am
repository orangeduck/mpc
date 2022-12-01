library_includedir      = $(includedir)
library_include_HEADERS = mpc.h

lib_LTLIBRARIES    = libmpc.la
libmpc_la_SOURCES  = mpc.c mpc.h
libmpc_la_CFLAGS   = -std=gnu99
libmpc_la_CFLAGS  += -W -Wall -Wextra -Wundef -Wunused -Wstrict-prototypes
libmpc_la_CFLAGS  += -Werror-implicit-function-declaration -Wshadow -Wcast-qual
libmpc_la_LDFLAGS  = $(AM_LDFLAGS) -version-info 1:2:0