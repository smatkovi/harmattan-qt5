
INCLUDES = -I../src @GLIB_CFLAGS@ @GCONF_CFLAGS@ @OSSO_IC_CFLAGS@
LDFLAGS = @GLIB_LIBS@ @GCONF_LIBS@ @OSSO_IC_LIBS@

bin_PROGRAMS = conn_settings

conn_settings_SOURCES = cmdline_conn_settings.c $(top_builddir)/src/conn_settings.h $(top_builddir)/src/conn_settings.c
conn_settings_LDADD = 

MAINTAINERCLEANFILES = Makefile.in
