include(../common_top.pri)

SRC_PREFIX = ../../src/batteryapplet
STUB_PREFIX = ../stubs

INCLUDEPATH = \
        $$STUB_PREFIX \
        $$SRC_PREFIX \
        $$INCLUDEPATH

QT += \
    testlib

TEMPLATE = app

DEFINES += \
    UNIT_TEST 

TARGET = ut_batteryapplet
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui \
    meegotouchcore \
    plugin \
    duicontrolpanel \

HEADERS += \
    $$STUB_PREFIX/qmbattery.h \
    $$STUB_PREFIX/qmdevicemode.h \
    ../../src/debug.h \
    ut_batteryapplet.h \
    $$SRC_PREFIX/batteryapplet.h \
    $$SRC_PREFIX/batterybusinesslogic.h \
    $$SRC_PREFIX/batteryimage.h \
    $$SRC_PREFIX/batterywidget.h \
    $$SRC_PREFIX/dcpbattery.h \
    $$SRC_PREFIX/slidercontainer.h \
    $$SRC_PREFIX/percentagecontainer.h

SOURCES += \
    $$STUB_PREFIX/qmbattery.cpp \
    $$STUB_PREFIX/qmdevicemode.cpp \
    ../../src/debug.cpp \
    ut_batteryapplet.cpp \
    $$SRC_PREFIX/batteryapplet.cpp \
    $$SRC_PREFIX/batterybusinesslogic.cpp \
    $$SRC_PREFIX/batteryimage.cpp \
    $$SRC_PREFIX/batterywidget.cpp \
    $$SRC_PREFIX/slidercontainer.cpp \
    $$SRC_PREFIX/percentagecontainer.cpp

INSTALLS += target
