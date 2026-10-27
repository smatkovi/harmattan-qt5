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

TARGET = ut_batteryimage
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    meegotouchcore \

contains(DEFINES, HAVE_QMSYSTEM) {
    CONFIG += qmsystem2
}

HEADERS += \
    ut_batteryimage.h \
    $$SRC_PREFIX/../debug.h \
    $$SRC_PREFIX/batteryimage.h 

SOURCES += \
    ut_batteryimage.cpp \
    $$SRC_PREFIX/../debug.cpp \
    $$SRC_PREFIX/batteryimage.cpp 

INSTALLS += target
