DISTCHECK_CONFIGURE_FLAGS = --disable-doc --disable-coverage

AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

# the documentation needs asciidoc and friends (--disable-doc without)
if CONTEXTKIT_BUILD_DOCS
DOC_SUBDIRS = doc
endif

SUBDIRS = \
	libcontextprovider \
	libcontextsubscriber \
	spec \
	$(DOC_SUBDIRS) \
	python \
	tools \
	common

DISTCLEANFILES = lcov.info lcov.html doltlibtool doltcompile
MAINTAINERCLEANFILES = INSTALL

check-sum:
	$(MAKE) -C libcontextprovider/unit-tests check-sum
	$(MAKE) -C libcontextsubscriber/unit-tests check-sum

check-customer:
	$(MAKE) -C libcontextprovider/customer-tests check-customer
	$(MAKE) -C libcontextsubscriber/customer-tests check-customer

coverage: covoptioncheck
	$(MAKE) -C libcontextsubscriber coverage \
	&& $(MAKE) -C libcontextprovider coverage

.PHONY: coverage check-customer check-sum

EXTRA_DIST = HACKING INSTALL.real MAINTAINERS autogen.sh autoclean.sh	\
	     ltmain.sh.as-needed.patch

include $(top_srcdir)/am/covoptioncheck.am
