dnl Usage: PKG_CHECK_MODULES_SUBST([name], [package to check for])
AC_DEFUN([PKG_CHECK_MODULES_SUBST], [
PKG_CHECK_MODULES([$1], [$2])
AC_SUBST([$1_CFLAGS])
AC_SUBST([$1_LIBS])])

dnl MOC may be given (a cross build's, the build machine's); test -x, as
dnl AC_CHECK_FILE refuses to run when cross compiling
AC_DEFUN([AC_PROG_QT_MOC], [
AS_IF([test -z "$MOC"], [MOC=`pkg-config Qt5Core --variable=host_bins`/moc])
AS_IF([test -x "$MOC"], [],
   [AC_MSG_ERROR([Can't find moc compiler: $MOC])])
AC_SUBST([MOC])])

dnl UIC may be given (a cross build's, the build machine's); test -x, as
dnl AC_CHECK_FILE refuses to run when cross compiling
AC_DEFUN([AC_PROG_QT_UIC], [
AS_IF([test -z "$UIC"], [UIC=`pkg-config Qt5Core --variable=host_bins`/uic])
AS_IF([test -x "$UIC"], [],
   [AC_MSG_ERROR([Can't find uic compiler: $UIC])])
AC_SUBST([UIC])])

dnl RCC may be given (a cross build's, the build machine's); test -x, as
dnl AC_CHECK_FILE refuses to run when cross compiling
AC_DEFUN([AC_PROG_QT_RCC], [
AS_IF([test -z "$RCC"], [RCC=`pkg-config Qt5Core --variable=host_bins`/rcc])
AS_IF([test -x "$RCC"], [],
   [AC_MSG_ERROR([Can't find rcc compiler: $RCC])])
AC_SUBST([RCC])])
