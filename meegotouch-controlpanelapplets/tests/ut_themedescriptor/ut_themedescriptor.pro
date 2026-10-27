include(../common_top.pri)

SRC_PREFIX = ../../src/themeapplet
STUB_PREFIX = ../stubs

INCLUDEPATH = \
        $$STUB_PREFIX \
	$$SRC_PREFIX \
	$$INCLUDEPATH

CONFIG += \
	gui \
	meegotouchcore \
	debug \

QT += \
    testlib \
    gui \
    dbus 

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_themedescriptor
target.path = /usr/lib/$$TEST_PKG_NAME

HEADERS += \
    $$STUB_PREFIX/mdesktopentry.h \
    ../../src/debug.h \
    ut_themedescriptor.h \
    $$SRC_PREFIX/themedescriptor.h

SOURCES += \
    $$STUB_PREFIX/mdesktopentry.cpp \
    ../../src/debug.cpp \
    ut_themedescriptor.cpp \
    $$SRC_PREFIX/themedescriptor.cpp

INSTALLS += target
