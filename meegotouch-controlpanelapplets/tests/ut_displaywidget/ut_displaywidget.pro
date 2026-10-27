include(../common_top.pri)

SRC_PREFIX = ../../src/displayapplet
STUB_PREFIX = ../stubs

INCLUDEPATH = \
    $$STUB_PREFIX \
    $$SRC_PREFIX \
    $$INCLUDEPATH

CONFIG += \
    gui \
    meegotouchcore \
    plugin \
    duicontrolpanel 

QT += \
    testlib \
    dbus \
    gui

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_displaywidget
target.path = /usr/lib/$$TEST_PKG_NAME

HEADERS += \
    $$STUB_PREFIX/qmdisplaystate.h \
    $$STUB_PREFIX/qmdevicemode.h \
    ../../src/debug.h \
    ut_displaywidget.h \
    $$SRC_PREFIX/displaybusinesslogic.h \
    $$SRC_PREFIX/displaywidget.h

SOURCES += \
    $$STUB_PREFIX/qmdisplaystate.cpp \
    $$STUB_PREFIX/qmdevicemode.cpp \
    ../../src/debug.cpp \
    ut_displaywidget.cpp \
    $$SRC_PREFIX/displaybusinesslogic.cpp \
    $$SRC_PREFIX/displaywidget.cpp

INSTALLS += target
