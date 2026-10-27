include(../common_top.pri)

SRC_PREFIX = ../../src/displayapplet
STUB_PREFIX = ../stubs

INCLUDEPATH = \
        $$STUB_PREFIX \
	$$SRC_PREFIX \
	$$INCLUDEPATH

QT += \
    testlib \
    dbus 

TEMPLATE = app

DEFINES += \
	UNIT_TEST 

TARGET = ut_displayapplet
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
	gui \
	meegotouchcore \
	plugin \
	duicontrolpanel \

HEADERS += \
    $$STUB_PREFIX/qmdisplaystate.h \
    $$STUB_PREFIX/qmdevicemode.h \
    ../../src/debug.h \
    ut_displayapplet.h \
    $$SRC_PREFIX/dcpdisplay.h \
    $$SRC_PREFIX/displayapplet.h \
    $$SRC_PREFIX/brightnessbrief.h \
    $$SRC_PREFIX/displaybusinesslogic.h \
    $$SRC_PREFIX/displaywidget.h


SOURCES += \
    $$STUB_PREFIX/qmdisplaystate.cpp \
    $$STUB_PREFIX/qmdevicemode.cpp \
    ../../src/debug.cpp \
    ut_displayapplet.cpp \
    $$SRC_PREFIX/displayapplet.cpp \
    $$SRC_PREFIX/brightnessbrief.cpp \
    $$SRC_PREFIX/displaybusinesslogic.cpp \
    $$SRC_PREFIX/displaywidget.cpp

INSTALLS += target
