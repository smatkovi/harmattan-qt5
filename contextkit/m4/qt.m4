dnl Usage: PKG_CHECK_MODULES_SUBST([name], [package to check for])
AC_DEFUN([PKG_CHECK_MODULES_SUBST], [
PKG_CHECK_MODULES([$1], [$2])
AC_SUBST([$1_CFLAGS])
AC_SUBST([$1_LIBS])])

AC_DEFUN([AC_PROG_QT_MOC], [
MOC=`pkg-config Qt5Core --variable=host_bins`/moc
AC_CHECK_FILE([$MOC], [],
   AC_MSG_ERROR([Can't find moc compiler: $MOC]))
AC_SUBST([MOC])])

AC_DEFUN([AC_PROG_QT_UIC], [
UIC=`pkg-config Qt5Core --variable=host_bins`/uic
AC_CHECK_FILE([$UIC], [],
   AC_MSG_ERROR([Can't find uic compiler: $UIC]))
AC_SUBST([UIC])])

AC_DEFUN([AC_PROG_QT_RCC], [
RCC=`pkg-config Qt5Core --variable=host_bins`/rcc
AC_CHECK_FILE([$RCC], [],
   AC_MSG_ERROR([Can't find rcc compiler: $RCC]))
AC_SUBST([RCC])])
