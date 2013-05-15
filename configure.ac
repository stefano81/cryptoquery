#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([cryptoquery], [0.0.1], [stefano.braghin@gmail.com])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([src/config.h])

default_macports_path=/opt/local
default_fink_path=/sw
case $host_os in
     darwin*) 
              # Perhaps this will be useful some time.
     	      #dnl nested function support
     	      #AC_MSG_NOTICE([Adding -fnested-functions to CFLAGS])
	          #CFLAGS="-fnested-functions"

#dnl fink or macports installation
     	      AC_MSG_CHECKING([for a fink or macports installation at $default_fink_path and $default_macports_path])
     	      if test -d $default_fink_path; then 
	      	 AC_MSG_RESULT([found fink!])
		 AC_MSG_NOTICE([Adding -I$default_fink_path/include to CFLAGS])
	      	 CFLAGS="-I$default_fink_path/include $CFLAGS"
		 AC_MSG_NOTICE([Adding -L$default_fink_path/lib to LDFLAGS])
		 LDFLAGS="-L$default_fink_path/lib $LDFLAGS"
              elif test -d $default_macports_path; then
		 AC_MSG_RESULT([found macports!])
		 AC_MSG_NOTICE([Adding -I$default_macports_path/include to CFLAGS])
	      	 CFLAGS="-I$default_macports_path/include $CFLAGS"
		 AC_MSG_NOTICE([Adding -L$default_macports_path/lib to LDFLAGS])
		 LDFLAGS="-L$default_macports_path/lib $LDFLAGS"
	      else
	 	 AC_MSG_RESULT(none)
		 AC_MSG_NOTICE([You may need to add set CPPFLAGS and LDFLAGS for gmp, etc.])
	      fi
	      ;;
esac


# Checks for programs.
AC_PROG_CC_C99

# Checks for libraries.
AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB(gmp, __gmpz_mul_si, ,
       [AC_MSG_ERROR([GNU MP not found, or not 3.1 or up, see http://gmplib.org/])])
AC_CHECK_LIB( [pbc], [pbc_set_msg_to_stderr], [], [AC_MSG_ERROR([PBC is required!])])

AC_CHECK_LIB([sqlite3], [sqlite3_open], [], [AC_MSG_ERROR([SQLite is required!])])

# Checks for header files.
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AM_INIT_AUTOMAKE([1.12 foreign no-define])

AC_CONFIG_FILES([Makefile
	src/Makefile])

AC_OUTPUT

# Print configuration to the user.
echo -ne "\n"
echo "global build variables"
echo "-----------------------------------------"
echo `date`
echo "host info:        $host"
echo "optimized build:  $with_enable_optimized"
echo "LDFLAGS:          $LDFLAGS"
echo "CPPFLAGS:         $CPPFLAGS"
echo "CFLAGS:           $CFLAGS"
echo "ABI:              $ABI"
#echo "CC:               $CC"
#echo "CXX:              $CXX"
echo "-----------------------------------------"
echo -ne "\n"

