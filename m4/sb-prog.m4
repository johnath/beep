# sb-prog.m4 - check for program, but let user override it   -*- Autoconf -*-
# serial 2
dnl
dnl
dnl
AC_DEFUN([SB_PROG], [dnl
AC_ARG_VAR([$1],   [$2 program])
AS_IF([test x = "x$][$1]["], [dnl
AC_PATH_PROG([$1], [$2], [false])
])
])dnl
dnl
dnl
dnl
