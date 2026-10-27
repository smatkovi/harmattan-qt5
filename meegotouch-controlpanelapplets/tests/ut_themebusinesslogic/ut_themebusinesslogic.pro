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
TARGET = ut_themebusinesslogic
target.path = /usr/lib/$$TEST_PKG_NAME

load(meegotouch_defines)
DEFINES += \
    MEEGO_THEMEDIR=\\\"\"$$M_THEME_DIR\"\\\"

HEADERS += \
    $$STUB_PREFIX/mdesktopentry.h \
    ../../src/debug.h \
    ut_themebusinesslogic.h \
    $$STUB_PREFIX/qdirstub.h \
    $$SRC_PREFIX/themebusinesslogic.h \
    $$SRC_PREFIX/themedescriptor.h

SOURCES += \
    $$STUB_PREFIX/mdesktopentry.cpp \
    ../../src/debug.cpp \
    ut_themebusinesslogic.cpp \
    $$SRC_PREFIX/themebusinesslogic.cpp \
    $$SRC_PREFIX/themedescriptor.cpp

INSTALLS += target
