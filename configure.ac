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
AC_CHECK_LIB([m], [pow])
AC_CHECK_LIB(gmp, __gmpz_mul_si, ,
       [AC_MSG_ERROR([GNU MP not found, or not 3.1 or up, see http://gmplib.org/])])
AC_CHECK_LIB( [pbc], [pbc_set_msg_to_stderr], [], [AC_MSG_ERROR([PBC is required!])])

#AC_CHECK_LIB([sqlite3], [sqlite3_open], [], [AC_MSG_ERROR([SQLite is required!])])
#AC_CHECK_LIB([readline], [readline], [], [AC_MSG_ERROR([readline is required!])])
#AC_CHECK_LIB([crypto], [AES_encrypt], [],[AC_MSG_ERROR([openssl/crypto (AES) is required!])])

# Checks for header files.
AC_CHECK_HEADERS([stdio.h])
AC_CHECK_HEADERS([stdlib.h])
AC_CHECK_HEADERS([string.h])
#AC_CHECK_HEADERS([unistd.h])
#AC_CHECK_HEADERS([readline/readline.h])
#AC_CHECK_HEADERS([readline/history.h])
AC_CHECK_HEADERS([arpa/inet.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC

AM_INIT_AUTOMAKE([1.11 foreign no-define])

AC_CONFIG_FILES([Makefile
	src/Makefile])

AC_OUTPUT

dnl # Print configuration to the user.
dnl echo -ne "\n"
dnl echo "global build variables"
dnl echo "-----------------------------------------"
dnl echo `date`
dnl echo "host info:        $host"
dnl echo "optimized build:  $with_enable_optimized"
dnl echo "LDFLAGS:          $LDFLAGS"
dnl echo "CPPFLAGS:         $CPPFLAGS"
dnl echo "CFLAGS:           $CFLAGS"
dnl echo "ABI:              $ABI"
dnl #echo "CC:               $CC"
dnl #echo "CXX:              $CXX"
dnl echo "-----------------------------------------"
dnl echo -ne "\n"

