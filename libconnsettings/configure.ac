dnl the version from debian/changelog's first line (dpkg-parsechangelog is
dnl not everywhere: FreeBSD, other distributions)
AC_INIT([libconnsettings], patsubst(esyscmd([sed -n '1s/.*(\(.*\)).*/\1/p' debian/changelog]), [
]), [jukka.rissanen@nokia.com])
AM_INIT_AUTOMAKE([foreign])

AC_PROG_CC
AC_PROG_LIBTOOL

AC_ARG_ENABLE(docs,     [ --enable-docs      Build DOXYGEN documentation (requires Doxygen)],enable_docs=$enableval,enable_docs=auto)

AC_PATH_PROG(DOXYGEN, doxygen, no)
AC_MSG_CHECKING([whether to build Doxygen documentation])

if test x$DOXYGEN = xno ; then
    have_doxygen=no
else
    have_doxygen=yes
fi
if test x$enable_docs = xauto ; then
    if test x$have_doxygen = xno ; then
        enable_docs=no
    else
        enable_docs=yes
    fi
fi
if test x$enable_docs = xyes; then
    if test x$have_doxygen = xno; then
        AC_MSG_ERROR([Building Doxygen docs explicitly required, but Doxygen not found])
    else 
	AC_MSG_RESULT(yes)
    fi
else
    AC_MSG_RESULT(no)
fi
 
AM_CONDITIONAL(DOXYGEN_DOCS_ENABLED, test x$enable_docs = xyes)
AC_SUBST(DOXYGEN)

# (gobject-2.0 for g_object_unref() on the GConf client)
PKG_CHECK_MODULES(GLIB, glib-2.0 gobject-2.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

PKG_CHECK_MODULES(GCONF, gconf-2.0)
AC_SUBST(GCONF_CFLAGS)
AC_SUBST(GCONF_LIBS)

PKG_CHECK_MODULES(OSSO_IC, osso-ic)
AC_SUBST(OSSO_IC_CFLAGS)
AC_SUBST(OSSO_IC_LIBS)

# For testing
# the unit tests need check; the library does not
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4], [have_check=yes], [have_check=no])
AM_CONDITIONAL(HAVE_CHECK, test x$have_check = xyes)

CONCFLAGS="-Wall -Werror -Wmissing-prototypes"
AC_SUBST(CONCFLAGS)

AC_CONFIG_FILES([Makefile \
	src/Makefile \
	tests/Makefile \
	cmdline/Makefile \
	connsettings.pc \
	doxygen.cfg])
AC_OUTPUT
