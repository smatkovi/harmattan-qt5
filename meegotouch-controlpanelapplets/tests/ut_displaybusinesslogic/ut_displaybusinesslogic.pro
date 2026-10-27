include(../common_top.pri)
include(../../localconfig.pri)

SRC_PREFIX = ../../src/displayapplet
STUB_PREFIX = ../stubs

INCLUDEPATH = \
    $$STUB_PREFIX \
    $$SRC_PREFIX \
    $$INCLUDEPATH

CONFIG += \
    gui \
    meegotouchcore \

QT += \
    testlib \
    dbus \
    gui

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_displaybusinesslogic
target.path = /usr/lib/$$TEST_PKG_NAME

HEADERS += \
    $$STUB_PREFIX/qmdisplaystate.h \
    $$STUB_PREFIX/qmdevicemode.h \
    ../../src/debug.h \
    ut_displaybusinesslogic.h \
    $$SRC_PREFIX/displaybusinesslogic.h

SOURCES += \
    $$STUB_PREFIX/qmdisplaystate.cpp \
    $$STUB_PREFIX/qmdevicemode.cpp \
    ../../src/debug.cpp \
    ut_displaybusinesslogic.cpp \
    $$SRC_PREFIX/displaybusinesslogic.cpp

INSTALLS += target
