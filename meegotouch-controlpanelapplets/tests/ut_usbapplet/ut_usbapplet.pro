include(../common_top.pri)

SRC_PREFIX = ../../src/usbapplet
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

TARGET = ut_usbapplet
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
	gui \
	meegotouchcore \
	plugin \
	duicontrolpanel \

HEADERS += \
    $$STUB_PREFIX/qmusbmode.h \
    ../../src/debug.h \
    ut_usbapplet.h \
    $$SRC_PREFIX/usbapplet.h \
    $$SRC_PREFIX/usbview.h

SOURCES += \
    $$STUB_PREFIX/qmusbmode.cpp \
    ../../src/debug.cpp \
    ut_usbapplet.cpp \
    $$SRC_PREFIX/usbapplet.cpp \
    $$SRC_PREFIX/usbview.cpp


INSTALLS += target
