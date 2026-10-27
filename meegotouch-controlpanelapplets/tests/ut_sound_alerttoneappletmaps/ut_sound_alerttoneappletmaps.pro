include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs
INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX
QT += \
    testlib

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_sound_alerttoneappletmaps
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    debug

HEADERS += \
    ut_alerttoneappletmaps.h \
    $$SRC_PREFIX/../debug.h \
    $$SRC_PREFIX/alerttoneappletmaps.h

SOURCES += \
    ut_alerttoneappletmaps.cpp \
    $$SRC_PREFIX/../debug.cpp \
    $$SRC_PREFIX/alerttoneappletmaps.cpp

INSTALLS += target

