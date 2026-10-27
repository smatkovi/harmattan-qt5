
INCLUDES = \
	@GLIB_CFLAGS@ \
	@GCONF_CFLAGS@ \
	@OSSO_IC_CFLAGS@ \
	@CHECK_CFLAGS@

LDFLAGS = \
	@GLIB_LIBS@ \
	@GCONF_LIBS@ \
	@OSSO_IC_LIBS@ \
	@CHECK_LIBS@ \
	$(top_builddir)/src/libconnsettings.la

AM_LDFLAGS = -static
#AM_CFLAGS = -Wall -Werror -Wmissing-prototypes

check_PROGRAMS = check_conn_settings check_memory_leaks
checkdir = lib/libconnsettings0-test

check_conn_settings_SOURCES = check_conn_settings.c $(top_builddir)/src/conn_settings.h
check_memory_leaks_SOURCES = check_memory_leaks.c $(top_builddir)/src/conn_settings.h

MAINTAINERCLEANFILES = Makefile.in
