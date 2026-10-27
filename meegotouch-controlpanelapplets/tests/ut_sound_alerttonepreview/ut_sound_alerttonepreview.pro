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
TARGET = ut_sound_alerttonepreview
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    plugin          \
    gui             \
    meegotouchcore  \
    debug           \
    link_pkgconfig  \
    timed           \
    duicontrolpanel

PKGCONFIG += gstreamer-0.10 profile

contains(DEFINES, HAVE_LIBRESOURCEQT) {
    PKGCONFIG += libresourceqt1
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

HEADERS +=                                    \
    ut_alerttonepreview.h                     \
    $$SRC_PREFIX/../debug.h                   \
    $$SRC_PREFIX/soundsettingsutils.h         \
    $$SRC_PREFIX/trackerconnection.h \
    $$SRC_PREFIX/qprofilevalue.h \
    $$SRC_PREFIX/qtrackedvariant.h \
    $$SRC_PREFIX/alerttonepreview.h



SOURCES +=                                    \
    ut_alerttonepreview.cpp                   \
    $$SRC_PREFIX/../debug.cpp                 \
    $$SRC_PREFIX/soundsettingsutils.cpp       \
    $$SRC_PREFIX/trackerconnection.cpp \
    $$SRC_PREFIX/qprofilevalue.cpp \
    $$SRC_PREFIX/qtrackedvariant.cpp \
    $$SRC_PREFIX/alerttonepreview.cpp



INSTALLS += target



