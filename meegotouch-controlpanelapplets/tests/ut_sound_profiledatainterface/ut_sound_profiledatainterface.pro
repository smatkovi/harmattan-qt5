include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs

INCLUDEPATH += \
    $$SRC_PREFIX \
    $$STUB_PREFIX

QT += \
    testlib \
    dbus 

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_sound_profiledatainterface
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui \
    meegotouchcore \
    link_pkgconfig \

HEADERS += \
    ../../src/debug.h \
    ut_profiledatainterface.h \
    $$SRC_PREFIX/profilebackend.h \
    $$SRC_PREFIX/profiledatainterface.h

SOURCES += \
    $$STUB_PREFIX/profilebackend_stub.cpp \
    ../../src/debug.cpp \
    ut_profiledatainterface.cpp \
    $$SRC_PREFIX/profiledatainterface.cpp

INSTALLS += target
