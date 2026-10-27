include(../common_top.pri)

SRC_PREFIX = ../../src/aboutapplet
STUB_PREFIX = ../stubs

INCLUDEPATH = \
        $$STUB_PREFIX \
        $$SRC_PREFIX \
        $$INCLUDEPATH

CONFIG += \
        gui \
        meegotouchcore \
        mobility \
        debug \
        link_pkgconfig \

QT += \
    testlib \
    gui \
    dbus 

MOBILITY += \
    systeminfo

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_aboutbusinesslogic
target.path = /usr/lib/$$TEST_PKG_NAME

HEADERS += \
    ../../src/debug.h \
    ut_aboutbusinesslogic.h \
    $$SRC_PREFIX/aboutbusinesslogic.h 

SOURCES += \
    ../../src/debug.cpp \
    ut_aboutbusinesslogic.cpp \
    $$SRC_PREFIX/aboutbusinesslogic.cpp 

INSTALLS += target
