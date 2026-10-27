include(../common_top.pri)
include(../../localconfig.pri)

SRC_PREFIX = ../../src/aboutapplet
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

DEFINES += LICENSE_PATH=\\\"\\\"
TARGET = ut_aboutapplet
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui \
    meegotouchcore \
    mobility \
    duicontrolpanel \
    link_pkgconfig

contains(DEFINES, HAVE_CONTENT_ACTION) {
    PKGCONFIG += contentaction-0.1
}

MOBILITY += systeminfo

HEADERS +=                               \
    ../../src/debug.h                    \
    ut_aboutapplet.h                     \
    $$SRC_PREFIX/logowidget.h            \
    $$SRC_PREFIX/aboutapplet.h           \
    $$SRC_PREFIX/aboutbusinesslogic.h    \
    $$SRC_PREFIX/aboutwidget.h

SOURCES +=                               \
    ../../src/debug.cpp                  \
    ut_aboutapplet.cpp                   \
    $$SRC_PREFIX/logowidget.cpp          \
    $$SRC_PREFIX/aboutapplet.cpp         \
    $$SRC_PREFIX/aboutbusinesslogic.cpp  \
    $$SRC_PREFIX/aboutwidget.cpp

INSTALLS += target

