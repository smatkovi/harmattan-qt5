
INCLUDES = -I../src @GLIB_CFLAGS@ @GCONF_CFLAGS@ @OSSO_IC_CFLAGS@

bin_PROGRAMS = conn_settings

conn_settings_SOURCES = cmdline_conn_settings.c $(top_builddir)/src/conn_settings.h $(top_builddir)/src/conn_settings.c
# (the libraries after the objects: in LDFLAGS today's linkers drop them)
conn_settings_LDADD = @GLIB_LIBS@ @GCONF_LIBS@ @OSSO_IC_LIBS@

MAINTAINERCLEANFILES = Makefile.in
