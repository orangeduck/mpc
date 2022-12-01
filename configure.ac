AC_INIT(mpc, 0.9.0, https://github.com/orangeduck/mpc/issues)
AC_CONFIG_AUX_DIR(build)
AM_INIT_AUTOMAKE([1.11 foreign dist-xz])
AM_SILENT_RULES([yes])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/mpc.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([Makefile libmpc.pc src/Makefile])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
LT_INIT

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h stdio.h stdarg.h string.h math.h errno.h ctype.h])

# Checks for library functions.
AC_PROG_GCC_TRADITIONAL

#
# Available features
#
AC_ARG_ENABLE([examples],
	[AC_HELP_STRING([--enable-examples], [Build examples/ directory])],
	[], [enable_examples=no])

#
# Check what features have been enabled
#
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" = yes])

# Generate all files
AC_OUTPUT