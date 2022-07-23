#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([beep],
        [1.4.13],
        [https://github.com/spkr-beep/beep/issues],
        [beep],
        [https://github.com/spkr-beep/beep])
AC_CONFIG_AUX_DIR([auto-aux])
AC_CONFIG_MACRO_DIR([auto-m4])

dnl We are not actually using Automake for the build system. This is
dnl only to have "autoreconf" make sure that aclocal.m4 is updated and
dnl config.{guess,sub} install-sh are added to auto-aux/.
AM_INIT_AUTOMAKE([
  1.16
  -Wall
  -Werror
  foreign
])

AC_CONFIG_SRCDIR([beep-main.c])
AC_CONFIG_HEADERS([auto-config.h])

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC
AC_PROG_GREP
AC_PROG_EGREP
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P
AC_PROG_SED

dnl Catch our custom macros
m4_pattern_forbid([^SB_])dnl

SB_PROG([DIFF],     [diff])
SB_PROG([DIFFSTAT], [diffstat])
SB_PROG([DOXYGEN],  [doxygen])
SB_PROG([GIT],      [git])
SB_PROG([INSTALL],  [install])
SB_PROG([PANDOC],   [pandoc])
SB_PROG([PYTHON3],  [python3])
SB_PROG([TAR],      [tar])
SB_PROG([TPUT],     [tput])

AC_SUBST([MKDIR_P], ["mkdir -p"])

AC_SUBST([INSTALL_DATA],    ["\${INSTALL} -m 0644 -p"])
AC_SUBST([INSTALL_DIR],     ["\${INSTALL} -m 0755 -d"])
AC_SUBST([INSTALL_PROGRAM], ["\${INSTALL} -m 0755 -p"])
AC_SUBST([INSTALL_SCRIPT],  ["\${INSTALL} -m 0755 -p"])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([
  fcntl.h
  limits.h
  stddef.h
  stdint.h
  stdlib.h
  string.h
  sys/ioctl.h
  sys/time.h
  unistd.h
])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([
  clock_gettime
  memset
  strdup
  strerror
  strrchr
  strtol
  strtoul
])

SB_SUMMARY_ITEM([abs top builddir], ["$(pwd)"])
SB_SUMMARY_ITEM([abs top srcdir],   ["$(cd "$(dirname "$0")" && pwd)"])
SB_SUMMARY_VAR([PWD])

AC_CONFIG_FILES([
  GNUmakefile
  Makefile
])
AC_CONFIG_FILES([stamp-h], [echo timestamp > stamp-h])
AC_OUTPUT
dnl
SB_SUMMARY
dnl
dnl
dnl Local Variables:
dnl indent-tabs-mode: nil
dnl End:
