#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([cryptoquery], [0.0.1], [stefano.braghin@gmail.com])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([src/config.h])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB([pbc], [element_initGT])
AC_CHECK_LIB([sqlite3], [sqlite3_open])

# Checks for header files.
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AM_INIT_AUTOMAKE([1.12 foreign no-define])

AC_CONFIG_FILES([Makefile
	src/Makefile])

AC_OUTPUT
