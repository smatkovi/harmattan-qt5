include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs
INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX
QT += \
    testlib \
    dbus

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_trackedvariant
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    plugin          \
    gui             \
    meegotouchcore  \
    debug           \
    link_pkgconfig  \
    timed           \
    duicontrolpanel

PKGCONFIG += dbus-1 profile

contains(DEFINES, HAVE_LIBRESOURCEQT) {
    PKGCONFIG += libresourceqt1
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

HEADERS +=                                    \
    ut_trackedvariant.h                       \
    $$SRC_PREFIX/../debug.h                   \
    $$SRC_PREFIX/soundsettingsutils.h         \
    $$SRC_PREFIX/trackerconnection.h \
    $$SRC_PREFIX/alerttone.h \
    $$SRC_PREFIX/qprofilevalue.h \
    $$SRC_PREFIX/qtrackedvariant.h


SOURCES +=                                    \
    ut_trackedvariant.cpp                     \
    $$SRC_PREFIX/../debug.cpp                 \
    $$SRC_PREFIX/soundsettingsutils.cpp       \
    $$SRC_PREFIX/trackerconnection.cpp \
    $$SRC_PREFIX/alerttone.cpp \
    $$SRC_PREFIX/qprofilevalue.cpp \
    $$SRC_PREFIX/qtrackedvariant.cpp

INSTALLS += target

