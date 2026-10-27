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
TARGET = ut_soundsettingsapplet
target.path = /usr/lib/$$TEST_PKG_NAME

DEFINES += M_APPLICATION_EXTENSION_DIR=""

CONFIG += \
    plugin          \
    gui             \
    meegotouchcore  \
    debug           \
    mobility        \
    link_pkgconfig  \
    duicontrolpanel

MOBILITY += \
    systeminfo

PKGCONFIG += dbus-1 profile ContentManager gstreamer-0.10

contains(DEFINES, HAVE_LIBRESOURCEQT) {
    PKGCONFIG += libresourceqt1
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

HEADERS +=                                  \
    ut_soundsettingsapplet.h                \
    $$SRC_PREFIX/../debug.h                 \
    $$SRC_PREFIX/soundsettingsutils.h       \
    $$SRC_PREFIX/drilldownitem.h            \
    $$SRC_PREFIX/trackerconnection.h        \
    $$SRC_PREFIX/alerttone.h                \
    $$SRC_PREFIX/profilebackend.h           \
    $$SRC_PREFIX/profiledatainterface.h     \
    $$SRC_PREFIX/profilecontainer.h         \
    $$SRC_PREFIX/alerttonewidget.h          \
    $$SRC_PREFIX/alerttonetoplevel.h        \
    $$SRC_PREFIX/alerttonepreview.h         \
    $$SRC_PREFIX/alerttoneappletmaps.h      \
    $$SRC_PREFIX/alerttoneappletwidget.h    \
    $$SRC_PREFIX/alerttonebrowser.h         \
    $$SRC_PREFIX/alerttonebrowsersheet.h    \
    $$SRC_PREFIX/alerttonedefaults.h        \
    $$SRC_PREFIX/alerttonedefaultsmodel.h   \
    $$SRC_PREFIX/qtrackedvariant.h          \
    $$SRC_PREFIX/qprofilevalue.h            \
    $$SRC_PREFIX/gconfstringcombo.h         \
    $$SRC_PREFIX/profileintcombo.h          \
    $$SRC_PREFIX/static.h                   \
    $$SRC_PREFIX/soundsettingsapplet.h      \
    $$SRC_PREFIX/alerttonebrowserstylable.h \
    $$SRC_PREFIX/profilewidgetcontainer.h

SOURCES +=                                  \
    ut_soundsettingsapplet.cpp              \
    $$SRC_PREFIX/../debug.cpp               \
    $$SRC_PREFIX/soundsettingsutils.cpp     \
    $$SRC_PREFIX/drilldownitem.cpp          \
    $$SRC_PREFIX/trackerconnection.cpp      \
    $$SRC_PREFIX/alerttone.cpp              \
    $$SRC_PREFIX/profilebackend.cpp         \
    $$SRC_PREFIX/profiledatainterface.cpp   \
    $$SRC_PREFIX/profilecontainer.cpp       \
    $$SRC_PREFIX/alerttonewidget.cpp        \
    $$SRC_PREFIX/alerttonetoplevel.cpp      \
    $$SRC_PREFIX/alerttonepreview.cpp       \
    $$SRC_PREFIX/alerttoneappletmaps.cpp    \
    $$SRC_PREFIX/alerttoneappletwidget.cpp  \
    $$SRC_PREFIX/alerttonebrowser.cpp       \
    $$SRC_PREFIX/alerttonebrowsersheet.cpp  \
    $$SRC_PREFIX/alerttonedefaults.cpp      \
    $$SRC_PREFIX/alerttonedefaultsmodel.cpp \
    $$SRC_PREFIX/qtrackedvariant.cpp        \
    $$SRC_PREFIX/qprofilevalue.cpp          \
    $$SRC_PREFIX/gconfstringcombo.cpp       \
    $$SRC_PREFIX/profileintcombo.cpp        \
    $$SRC_PREFIX/soundsettingsapplet.cpp    \
    $$SRC_PREFIX/alerttonebrowserstylable.cpp \
    $$SRC_PREFIX/profilewidgetcontainer.cpp

INSTALLS += target

