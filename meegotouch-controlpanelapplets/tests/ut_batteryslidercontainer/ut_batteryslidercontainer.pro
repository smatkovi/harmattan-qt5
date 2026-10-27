include(../common_top.pri)

SRC_PREFIX = ../../src/batteryapplet
STUB_PREFIX = ../stubs

INCLUDEPATH += \
    $$SRC_PREFIX

QT += testlib

TEMPLATE = app

DEFINES += UNIT_TEST 

TARGET = ut_batteryslidercontainer
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    meegotouchcore

HEADERS += \
    ut_slidercontainer.h \
    $$SRC_PREFIX/../debug.h \
    $$SRC_PREFIX/slidercontainer.h 

SOURCES += \
    ut_slidercontainer.cpp \
    $$SRC_PREFIX/../debug.cpp \
    $$SRC_PREFIX/slidercontainer.cpp 

INSTALLS += target
