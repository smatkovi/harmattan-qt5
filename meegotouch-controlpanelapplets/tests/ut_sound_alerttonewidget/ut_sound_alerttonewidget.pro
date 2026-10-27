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
TARGET = ut_sound_alerttonewidget
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    plugin          \
    gui             \
    meegotouchcore  \
    debug           \
    link_pkgconfig  \
    timed           \
    duicontrolpanel

PKGCONFIG += dbus-1 profile ContentManager gstreamer-0.10

contains(DEFINES, HAVE_LIBRESOURCEQT) {
    PKGCONFIG += libresourceqt1
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

HEADERS +=                                    \
    ut_alerttonewidget.h                      \
    $$SRC_PREFIX/../debug.h                   \
    $$SRC_PREFIX/soundsettingsutils.h         \
    $$SRC_PREFIX/drilldownitem.h \
    $$SRC_PREFIX/trackerconnection.h \
    $$SRC_PREFIX/alerttone.h \
    $$SRC_PREFIX/alerttonepreview.h \
    $$SRC_PREFIX/alerttonewidget.h  \
    $$SRC_PREFIX/alerttonetoplevel.h \
    $$SRC_PREFIX/alerttoneappletmaps.h \
    $$SRC_PREFIX/qprofilevalue.h \
    $$SRC_PREFIX/static.h \
    $$SRC_PREFIX/qtrackedvariant.h



SOURCES +=                                    \
    ut_alerttonewidget.cpp                    \
    $$SRC_PREFIX/../debug.cpp                 \
    $$SRC_PREFIX/soundsettingsutils.cpp       \
    $$SRC_PREFIX/drilldownitem.cpp \
    $$SRC_PREFIX/trackerconnection.cpp \
    $$SRC_PREFIX/alerttone.cpp \
    $$SRC_PREFIX/alerttonepreview.cpp \
    $$SRC_PREFIX/alerttonewidget.cpp \
    $$SRC_PREFIX/alerttonetoplevel.cpp \
    $$SRC_PREFIX/alerttoneappletmaps.cpp \
    $$SRC_PREFIX/qprofilevalue.cpp \
    $$SRC_PREFIX/qtrackedvariant.cpp



INSTALLS += target



