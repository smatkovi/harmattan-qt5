include(../shared.pri)
TEMPLATE = subdirs
addSubDirs(dummydriver_hwr)
addSubDirs(dummydriver_words)

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

QMAKE_CLEAN += build-stamp configure-stamp

