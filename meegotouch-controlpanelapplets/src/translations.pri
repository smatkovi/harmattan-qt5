#
# Translation generation project include file
# CATALOGNAME must be the applet source dir name
#

contains(BUILD_FEATURES,coverage) {
	QMAKE_EXTRA_TARGETS += coverage
	coverage.commands = $$system(true)
}

isEmpty(CATALOGNAME) {
    warning($$PWD does not set the CATALOGNAME!)
}

# hack around the meegotouch_translations.prf, if this isn't empty
# then the generated translations will not be prefixed with '\ !!'
LRELEASE_OPTIONS = -idbased

SOURCEDIR = $$PWD/$$CATALOGNAME
TRANSLATIONDIR = $$PWD
TRANSLATION_INSTALLDIR = /usr/share/l10n/meegotouch
# (load() finds the features also on QMAKEFEATURES, outside Qt's own
# mkspecs)
load(meegotouch_defines)
load(meegotouch_translations)

# hack around again?
LRELEASE_OPTIONS -= -markuntranslated '!!\\ '

