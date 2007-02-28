# aclocal.m4 generated automatically by aclocal 1.6.3 -*- Autoconf -*-

# Copyright 1996, 1997, 1998, 1999, 2000, 2001, 2002
# Free Software Foundation, Inc.
# This file is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


AC_DEFUN([AC_CHECK_WORKING_GETIFADDRS],
[
  AC_MSG_CHECKING([whether getifaddrs returns correct values])
  AC_RUN_IFELSE([AC_LANG_SOURCE([[
    #include <stdio.h>
    #if STDC_HEADERS
    # include <stdlib.h>
    # include <stddef.h>
    #else
    # if HAVE_STDLIB_H
    #  include <stdlib.h>
    # endif
    #endif
    #if HAVE_SYS_SOCKET_H
    # include <sys/socket.h>
    #endif
    #include <sys/types.h>
    #include <net/if.h>
    #include <ifaddrs.h>

    int main() {
        struct ifaddrs *net;
        if (getifaddrs(&net) != 0) return 1;
        if (net!=NULL)
            if (net->ifa_data==NULL) {
                freeifaddrs(net);
                return 1;
            }
        freeifaddrs(net);
        return 0;
    }
  ]])],[
    AC_MSG_RESULT(yes)
    AC_DEFINE_UNQUOTED([HAVE_WORKING_GETIFADDRS],[1],[getifaddrs works as espected])
	 INPUT_FOUND=1
	 INPUT_SYSTEM="$INPUT_SYSTEM getifaddrs.$OBJEXT"
  ],[
    AC_MSG_RESULT(no)
  ],[
    AC_MSG_RESULT(cross-compiling, assume yes)
    AC_DEFINE_UNQUOTED([HAVE_WORKING_GETIFADDRS],[1],[getifaddrs works as espected])
	 INPUT_FOUND=1
	 INPUT_SYSTEM="$INPUT_SYSTEM getifaddrs.$OBJEXT"
  ])
])


# AC_CHECK_CC_OPT
# ---------------------
# Check whether the C compiler accepts the given option
AC_DEFUN([AC_CHECK_CC_OPT],
  [AC_MSG_CHECKING([whether ${CC-cc} accepts -[$1]])
   echo 'void f(){}' > conftest.c
   if test -z "`${CC-cc} -c -$1 conftest.c 2>&1`"; then
     AC_MSG_RESULT(yes)
     CFLAGS="$CFLAGS -$1"
   else
     AC_MSG_RESULT(no)
   fi
   rm -f conftest*
])


# Usage:
#  AC_ASK_OPTLIB(library, function, header, description, packagename, libdefine, headerdefine)
#  Only the last argument is optional, because they will only be defined if both the lib
#  and header are found
#  Example:
#    AC_ASK_OPTLIB(z, compress2, zlib.h, [            Support zlib], zlib, HAVE_LIBZ, HAVE_ZLIB_H)
AC_DEFUN([AC_ASK_OPTLIB], [
  AC_ARG_WITH($5, [  --with-$5 $4], [
# Specified
    LIBSPEC=$withval
  ], [
# Not specified
    LIBSPECFLAGS=`pkg-config --libs $5 2>/dev/null`
    LIBSPECCFLAGS=`pkg-config --cflags $5 2>/dev/null`
    AC_CHECK_LIB($1, $2, [
      OLDCPPFLAGS="$CPPFLAGS"
      OLDCFLAGS="$CFLAGS"
      CPPFLAGS="$CPPFLAGS $LIBSPECCFLAGS"
      CFLAGS="$CFLAGS $LIBSPECCFLAGS"
      AC_CHECK_HEADER($3, [
        LIBSPEC=yes
      ], [
        LIBSPEC=no
      ])
      CPPFLAGS="$OLDCPPFLAGS"
      CFLAGS="$OLDCFLAGS"
    ], [
      LIBSPEC=no
dnl      AC_MSG_WARN(Didn't find $5)
    ], $LIBSPECFLAGS)
  ])
  case $LIBSPEC in
  	no)
dnl  		AC_MSG_WARN(Support for $5 disabled)
  		;;
  	*)
  		if test "${LIBSPEC}" = "yes"; then
			true
		else
			LIBSPECFLAGS="-L${LIBSPEC}/lib ${LIBSPECFLAGS}"
			LIBSPECCFLAGS="-I${LIBSPEC}/include ${LIBSPECCFLAGS}"
  		fi
		AC_CHECK_LIB($1, $2, [
		  OLDCFLAGS="$CFLAGS"
		  OLDCPPFLAGS="$CPPFLAGS"
		  CPPFLAGS="$CPPFLAGS ${LIBSPECCFLAGS}"
		  CFLAGS="$CFLAGS ${LIBSPECCFLAGS}"
  		  AC_CHECK_HEADER($3, [
		    if test -n "$7"; then
		      AC_DEFINE($7, [1], [Define to 1 if you have the <$3> header file.])
		    fi
		    if test -n "$6"; then
		      AC_DEFINE($6, [1], [Define to 1 if you have $2 from $5])
		    fi
		    LDFLAGS="$LDFLAGS $LIBSPECFLAGS"
		    LIBS="$LIBS -l$1"
		  ], [
		    CFLAGS="$OLDCFLAGS"
		    CPPFLAGS="$OLDCPPFLAGS"
		    AC_MSG_ERROR(Could not find $3)
		  ])
		], [
		  AC_MSG_ERROR(Could not find $5)
		], $LIBSPECFLAGS)
  		;;
  esac
])

AC_DEFUN([AC_QEF_C_NORETURN],
[AC_REQUIRE([AC_PROG_CC])
AC_MSG_CHECKING(whether the C compiler (${CC-cc}) accepts noreturn attribute)
AC_CACHE_VAL(qef_cv_c_noreturn,
[qef_cv_c_noreturn=no
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
void f (void) __attribute__ ((noreturn));
void f (void)
{
   exit (1);
   }
   ]], [[
      f ();
      ]])],[qef_cv_c_noreturn="yes";  FUNCATTR_NORETURN_VAL="__attribute__ ((noreturn))"],[qef_cv_c_noreturn="no";   FUNCATTR_NORETURN_VAL="/* will not return */"])
])

AC_MSG_RESULT($qef_cv_c_noreturn)
AC_DEFINE_UNQUOTED([FUNCATTR_NORETURN], [$FUNCATTR_NORETURN_VAL],[cc knows about noreturn])
])dnl

