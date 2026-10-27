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
TARGET = ut_sound_profilebackend
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui \
    meegotouchcore \
    link_pkgconfig \

contains(DEFINES, HAVE_LIBPROFILE) {
    PKGCONFIG += \
        profile \
        dbus-1
}

HEADERS += \
    ../../src/debug.h \
    ut_profilebackend.h \
    $$SRC_PREFIX/profilebackend.h

SOURCES += \
    ../../src/debug.cpp \
    ut_profilebackend.cpp \
    $$SRC_PREFIX/profilebackend.cpp

INSTALLS += target
