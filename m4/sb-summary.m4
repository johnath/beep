# sb-summary.m4 - print summary at the end of configure      -*- Autoconf -*-
# serial 3
dnl
dnl
m4_pattern_forbid([SB_SUMMARY_ITEM_WIDTH])dnl
AC_DEFUN([SB_SUMMARY_ITEM_WIDTH], [17])dnl
dnl
dnl
m4_pattern_forbid([SB_SUMMARY_FILE])dnl
AC_DEFUN([SB_SUMMARY_FILE], [sb_summary.txt])dnl
dnl
dnl
AC_DEFUN_ONCE([SB_SUMMARY_INIT], [dnl
printf '%s for %s-%s:\n' 'Build Summary' "$PACKAGE_TARNAME" "$PACKAGE_VERSION" > SB_SUMMARY_FILE
])dnl
dnl
dnl
AC_DEFUN([SB_SUMMARY_ITEM], [dnl
AC_REQUIRE([SB_SUMMARY_INIT])dnl
printf '  %-]SB_SUMMARY_ITEM_WIDTH[s %s\n' m4_default(["AS_ESCAPE($1)"],[""]) m4_default($2,[""]) >> SB_SUMMARY_FILE
])dnl
dnl
dnl
AC_DEFUN([SB_SUMMARY_VAR], [dnl
AC_REQUIRE([SB_SUMMARY_INIT])dnl
SB_SUMMARY_ITEM([$1], ["$][$1]["])dnl
])dnl
dnl
dnl
AC_DEFUN([SB_SUMMARY], [dnl
AC_REQUIRE([SB_SUMMARY_INIT])dnl
cat "SB_SUMMARY_FILE"
rm -f "SB_SUMMARY_FILE"
])dnl
dnl
dnl
