include(../common_top.pri)
include(../../localconfig.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs
INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX
QT += \
    testlib \
    dbus

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_sound_trackerconnection
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    meegotouchcore  \
    timed           \
    debug

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

HEADERS +=                                    \
    ut_trackerconnection.h                    \
    $$SRC_PREFIX/../debug.h                   \
    $$SRC_PREFIX/soundsettingsutils.h         \
    $$SRC_PREFIX/trackerconnection.h

SOURCES +=                                    \
    ut_trackerconnection.cpp                  \
    $$SRC_PREFIX/../debug.cpp                 \
    $$SRC_PREFIX/soundsettingsutils.cpp       \    
    $$SRC_PREFIX/trackerconnection.cpp

INSTALLS += target

