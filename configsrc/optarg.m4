# Usage:
#  DC_ASK_OPTLIB(library, function, header, description, packagename, libdefine, headerdefine)
#  Only the last argument is optional, because they will only be defined if both the lib
#  and header are found
#  Example:
#    DC_ASK_OPTLIB(z, compress2, zlib.h, [            Support zlib], zlib, HAVE_LIBZ, HAVE_ZLIB_H)
AC_DEFUN(DC_ASK_OPTLIB, [
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
