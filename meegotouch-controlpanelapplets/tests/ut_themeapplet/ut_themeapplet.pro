include(../common_top.pri)

MOC_DIR = .moc
SRC_PREFIX = ../../src/themeapplet
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

TARGET = ut_themeapplet
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
	gui \
	meegotouchcore \
	plugin \
	duicontrolpanel \

load(meegotouch_defines)
DEFINES += \
    MEEGO_THEMEDIR=\\\"\"$$M_THEME_DIR\"\\\"

HEADERS += \
    $$STUB_PREFIX/mdesktopentry.h \
    ../../src/debug.h \
    ut_themeapplet.h \
    $$SRC_PREFIX/drilldownitem.h \
    $$SRC_PREFIX/themeapplet.h \
    $$SRC_PREFIX/themebrief.h \
    $$SRC_PREFIX/themebusinesslogic.h \
    $$SRC_PREFIX/themecellcreator.h \
    $$SRC_PREFIX/themedescriptor.h \
    $$SRC_PREFIX/themedialog.h \
    $$SRC_PREFIX/themelistmodel.h \
    $$SRC_PREFIX/themewidget.h

SOURCES += \
    $$STUB_PREFIX/mdesktopentry.cpp \
    ../../src/debug.cpp \
    ut_themeapplet.cpp \
    $$SRC_PREFIX/drilldownitem.cpp \
    $$SRC_PREFIX/themeapplet.cpp \
    $$SRC_PREFIX/themebrief.cpp \
    $$SRC_PREFIX/themebusinesslogic.cpp \
    $$SRC_PREFIX/themecellcreator.cpp \
    $$SRC_PREFIX/themedescriptor.cpp \
    $$SRC_PREFIX/themedialog.cpp \
    $$SRC_PREFIX/themelistmodel.cpp \
    $$SRC_PREFIX/themewidget.cpp

INSTALLS += target
