ACLOCAL_AMFLAGS = -I m4
# t (the tests and demos) is built by make check (make -C t check): its
# programs are no part of the library
SUBDIRS = src doc data
DIST_SUBDIRS = src t doc data

check-local:
	$(MAKE) -C t check

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	contentaction-0.1.pc
noinst_DATA = \
	contentaction-0.1-uninstalled.pc
