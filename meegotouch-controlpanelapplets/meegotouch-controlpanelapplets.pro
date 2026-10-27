# ##################################################
# MeeGo Touch Controlpanel applets main projectfile
# ##################################################

system(./configure)
include(shared.pri)
TEMPLATE = subdirs

CONFIG(docs) {
    addSubDirs(doc)
}

addSubDirs(src)
addSubDirs(tests)

QMAKE_CLEAN += \
	configure-stamp \
	build-stamp

contains(BUILD_FEATURES,coverage) {
	QMAKE_EXTRA_TARGETS += coverage
	coverage.depends = src/Makefile
	coverage.commands = \
                cd tests && make coverage && cd .. \
                && genhtml --no-branch-coverage --legend -o coverage/ \
                   -t \"MeeGo Touch Controlpanel Applets Coverage Report\" \
                tests/ut_*/selected.cov
}

   

