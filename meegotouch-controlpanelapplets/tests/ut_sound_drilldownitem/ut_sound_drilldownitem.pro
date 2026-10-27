include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
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
    gui

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_sound_drilldownitem
target.path = /usr/lib/$$TEST_PKG_NAME

HEADERS += \
    ../../src/debug.h \
    ut_drilldownitem.h \
    $$SRC_PREFIX/drilldownitem.h

SOURCES += \
    ../../src/debug.cpp \
    ut_drilldownitem.cpp \
    $$SRC_PREFIX/drilldownitem.cpp

INSTALLS += target

