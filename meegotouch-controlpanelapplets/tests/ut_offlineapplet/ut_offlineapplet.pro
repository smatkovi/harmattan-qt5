include(../common_top.pri)

SRC_PREFIX = ../../src/offlineapplet
STUB_PREFIX = ../stubs
TOOLS_PREFIX = ../tools

#
# Sorry I made a qmdevicemode.h in the stub directory that causes some problems.
# FIXME: I need a better solution for this...
#
INCLUDEPATH = \
    /usr/include/qmsystem2/ \
    $$INCLUDEPATH \
    $$STUB_PREFIX \
    $$SRC_PREFIX \
    $$TOOLS_PREFIX

QT += \
    dbus \
    testlib

TEMPLATE = app

DEFINES += \
    UNIT_TEST 

TARGET = ut_offlineapplet
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui \
    meegotouchcore \
    plugin \
    duicontrolpanel \

#
# If we will use the QmSystem library we shall stub it here. 
#
contains(DEFINES, HAVE_QMSYSTEM) {
    CONFIG += qmsystem2
}

HEADERS += \
    ut_offlineapplet.h \
    $$SRC_PREFIX/offlineapplet.h \
    $$SRC_PREFIX/offlinebrief.h \
    ../../src/debug.h \
    $$STUB_PREFIX/stubbase.h \
    $$STUB_PREFIX/qmdevicemode_stub.h \
    $$TOOLS_PREFIX/signalchecker.h



SOURCES += \
    ut_offlineapplet.cpp \
    ../../src/debug.cpp \
    $$SRC_PREFIX/offlineapplet.cpp \
    $$SRC_PREFIX/offlinebrief.cpp \
    $$STUB_PREFIX/stubbase.cpp \
    $$TOOLS_PREFIX/signalchecker.cpp

INSTALLS += target
