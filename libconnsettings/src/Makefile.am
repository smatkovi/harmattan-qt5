DEFS = -DDBUS_API_SUBJECT_TO_CHANGE -DGCONF_DISABLE_DEPRECATED
INCLUDES = @GLIB_CFLAGS@ @GCONF_CFLAGS@ @OSSO_IC_CFLAGS@
LIBS = @GLIB_LIBS@ @GCONF_LIBS@ @OSSO_IC_LIBS@

#AM_CFLAGS = $(CONCFLAGS)

lib_LTLIBRARIES = libconnsettings.la
libconnsettings_la_SOURCES = conn_settings.c conn_settings.h
libconnsettings_la_LDFLAGS = -version-info 0:0:0

connsettingsincludedir = $(includedir)/connsettings
connsettingsinclude_HEADERS = conn_settings.h

MAINTAINERCLEANFILES = Makefile.in
