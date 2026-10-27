include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs

INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX
QT += \
    testlib \
    dbus

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_sound_profilecontainer
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui             \
    meegotouchcore

HEADERS += \
    ut_profilecontainer.h \
    $$SRC_PREFIX/profilecontainer.h \
    $$SRC_PREFIX/../debug.h

SOURCES += \
    ut_profilecontainer.cpp \
    $$SRC_PREFIX/profilecontainer.cpp \
    $$SRC_PREFIX/../debug.cpp

INSTALLS += target

