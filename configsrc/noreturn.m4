AC_DEFUN([QEF_C_NORETURN],
[AC_REQUIRE([AC_PROG_CC])
AC_MSG_CHECKING(whether the C compiler (${CC-cc}) accepts noreturn attribute)
AC_CACHE_VAL(qef_cv_c_noreturn,
[qef_cv_c_noreturn=no
AC_TRY_COMPILE(
[#include <stdio.h>
void f (void) __attribute__ ((noreturn));
void f (void)
{
   exit (1);
}
], [
   f ();
],
[qef_cv_c_noreturn="yes";  FUNCATTR_NORETURN_VAL="__attribute__ ((noreturn))"],
[qef_cv_c_noreturn="no";   FUNCATTR_NORETURN_VAL="/* will not return */"])
])

AC_MSG_RESULT($qef_cv_c_noreturn)
AC_DEFINE_UNQUOTED(FUNCATTR_NORETURN, $FUNCATTR_NORETURN_VAL)
])dnl
