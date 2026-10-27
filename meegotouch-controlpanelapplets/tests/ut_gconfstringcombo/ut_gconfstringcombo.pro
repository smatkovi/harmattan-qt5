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
TARGET = ut_gconfstringcombo
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

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

HEADERS +=                                  \
    ut_gconfstringcombo.h                   \
    $$SRC_PREFIX/../debug.h                 \
    $$SRC_PREFIX/soundsettingsutils.h       \
    $$SRC_PREFIX/trackerconnection.h        \
    $$SRC_PREFIX/alerttoneappletmaps.h      \
    $$SRC_PREFIX/qprofilevalue.h            \
    $$SRC_PREFIX/qtrackedvariant.h          \
    $$SRC_PREFIX/gconfstringcombo.h

SOURCES +=                                  \
    ut_gconfstringcombo.cpp                 \
    $$SRC_PREFIX/../debug.cpp               \
    $$SRC_PREFIX/soundsettingsutils.cpp     \
    $$SRC_PREFIX/trackerconnection.cpp      \
    $$SRC_PREFIX/alerttoneappletmaps.cpp    \
    $$SRC_PREFIX/qprofilevalue.cpp          \
    $$SRC_PREFIX/qtrackedvariant.cpp        \
    $$SRC_PREFIX/gconfstringcombo.cpp

INSTALLS += target

