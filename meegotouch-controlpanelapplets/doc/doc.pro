include(../shared.pri)

# Stupid qmake wants to link everything.
QMAKE_LINK = @: IGNORE THIS LINE

QMAKE_EXTRA_TARGETS += doc 
QMAKE_EXTRA_COMPILER += doc

QMAKE_CLEAN += html/*

## Generate the actual documenation by first running dependencies
doc.target        = ./html/index.html
doc.config       += no_link explicit_dependencies
doc.commands      = doxygen Doxyfile
doc.input         = $${doc.depends}
doc.output        = $${doc.target}
#doc.depends       = FORCE

## Make are doc target build by default If 'make' is executed in doc folder
PRE_TARGETDEPS += $${doc.output}

# Install rules
htmldocs.depends = doc
htmldocs.files = html/*
htmldocs.path = $$(DEBIAN_DESTDIR)/usr/share/libmeegocontrol-doc
htmldocs.CONFIG += no_link no_check_exist
INSTALLS += htmldocs

QMAKE_EXTRA_TARGETS += htmldocs
