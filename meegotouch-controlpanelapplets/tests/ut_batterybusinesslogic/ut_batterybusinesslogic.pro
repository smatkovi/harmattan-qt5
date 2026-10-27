include(../common_top.pri)

SRC_PREFIX = ../../src/batteryapplet
STUB_PREFIX = ../stubs

INCLUDEPATH = \
        $$STUB_PREFIX \
        $$SRC_PREFIX \
        $$INCLUDEPATH

QT += testlib

TEMPLATE = app

DEFINES += UNIT_TEST 

TARGET = ut_batterybusinesslogic
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    meegotouchcore \

HEADERS += \
    $$SRC_PREFIX/../debug.h \ 
    $$STUB_PREFIX/qmbattery.h \
    $$STUB_PREFIX/qmdevicemode.h \
    ut_batterybusinesslogic.h \
    $$SRC_PREFIX/batterybusinesslogic.h

SOURCES += \
    $$SRC_PREFIX/../debug.cpp \
    $$STUB_PREFIX/qmbattery.cpp \
    $$STUB_PREFIX/qmdevicemode.cpp \
    ut_batterybusinesslogic.cpp \
    $$SRC_PREFIX/batterybusinesslogic.cpp

INSTALLS += target
