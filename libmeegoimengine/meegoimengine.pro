include(shared.pri)
TARGET = meegoimengine
TEMPLATE = subdirs

addSubDirs(src)
addSubDirs(unit_test, src)
addSubDirs(dummydriver, src)

CONFIG(docs) {
    addSubDirs(doc)
}
include(meegoimengine.prf)

HEADERS = includes/mimenginetypes.h
headers.path += $$MEEGOIMENGINE_INSTALL_HEADERS
headers.files += $$HEADERS

install_headers.path = $$MEEGOIMENGINE_INSTALL_HEADERS
install_headers.files = include/*

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = meegoimengine.prf

INSTALLS += \
    headers \
    install_headers \
    install_prf \

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

QMAKE_EXTRA_TARGETS += memcheck
memcheck.target = memcheck
memcheck.CONFIG = recursive

QMAKE_CLEAN += build-stamp configure-stamp
