include(../common_top.pri)

SRC_PREFIX = ../../src/themeapplet
STUB_PREFIX = ../stubs

INCLUDEPATH += \
    $$SRC_PREFIX 

CONFIG += \
	gui \
	meegotouchcore \
	debug \

QT += \
    testlib \
    gui \
    dbus 

TEMPLATE = app
DEFINES += \
    UNIT_TEST \
    FUNCTIONAL_TEST

TARGET = ft_themebusinesslogic
target.path = /usr/lib/$$TEST_PKG_NAME

load(meegotouch_defines)
DEFINES += \
    MEEGO_THEMEDIR=\\\"\"$$M_THEME_DIR\"\\\"

HEADERS += \
    ../../src/debug.h \
    ft_themebusinesslogic.h \
    $$SRC_PREFIX/themebusinesslogic.h \
    $$SRC_PREFIX/themedescriptor.h

SOURCES += \
    ../../src/debug.cpp \
    ft_themebusinesslogic.cpp \
    $$SRC_PREFIX/themebusinesslogic.cpp \
    $$SRC_PREFIX/themedescriptor.cpp

INSTALLS += target
