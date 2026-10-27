include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs

INCLUDEPATH += \
    $$SRC_PREFIX

QT += \
    testlib \
    dbus 

TEMPLATE = app
TARGET = ft_sound_profiledatainterface
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui \
    meegotouchcore \
    link_pkgconfig \

contains(DEFINES, HAVE_LIBPROFILE) {
    PKGCONFIG += profile dbus-1
}

HEADERS += \
    ../../src/debug.h \
    ft_profiledatainterface.h \
    $$SRC_PREFIX/profilebackend.h \
    $$SRC_PREFIX/profiledatainterface.h

SOURCES += \
    ../../src/debug.cpp \
    ft_profiledatainterface.cpp \
    $$SRC_PREFIX/profilebackend.cpp \
    $$SRC_PREFIX/profiledatainterface.cpp

INSTALLS += target

