include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

TEMPLATE = lib
VERSION = 0.1.1
MOC_DIR = .moc
TARGET = meegocontrol
OBJECTS_DIR = .objects
DEFINES += BUILDING_LIBMEEGOCONTROL

QT += dbus

CONFIG +=          \
    dll            \
    gui            \
    meegotouchcore \
    link_pkgconfig \
    duicontrolpanel

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

#
# FIXME: These are needed for the soundssettingsapplet. We maybe could remove
# some of these, but for that we need to use the LIBMEEGOCONTROL define in the
# soundsettingsapplet source.
#
PKGCONFIG += dbus-1 profile gstreamer-0.10

###########################################
# soundsetting applet extra dependencies  #
###########################################
contains(DEFINES, HAVE_CONTENT_MANAGER) {
    CONFIG += contentmanager
}

contains(DEFINES, HAVE_LIBRESOURCEQT) {
    PKGCONFIG += libresourceqt1
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

#
# This way the code wil always know that it is running outside the controlpanel
#
DEFINES += \
    LIBMEEGOCONTROL

SOUNDSETTINGS_PATH = ../soundsettingsapplet

PUBLIC_HEADERS = \
    $$SOUNDSETTINGS_PATH/alerttone.h               \
    $$SOUNDSETTINGS_PATH/alerttonetoplevel.h       \
    $$SOUNDSETTINGS_PATH/alerttonebrowser.h        \
    $$SOUNDSETTINGS_PATH/alerttonebrowsersheet.h   \
    $$SOUNDSETTINGS_PATH/qprofilevalue.h           \
    $$SOUNDSETTINGS_PATH/qtrackedvariant.h         \
    init.h                                         \
    meegocontrolexport.h                           \
    meegocontrol.h

HEADERS =                                          \
    ../debug.h                                     \
    $$SOUNDSETTINGS_PATH/static.h                  \
    $$SOUNDSETTINGS_PATH/alerttonepreview.h        \
    $$SOUNDSETTINGS_PATH/trackerconnection.h       \
    $$SOUNDSETTINGS_PATH/drilldownitem.h           \
    $$SOUNDSETTINGS_PATH/alerttoneappletmaps.h     \
    $$SOUNDSETTINGS_PATH/alerttonedefaultsmodel.h  \
    $$SOUNDSETTINGS_PATH/soundsettingsutils.h      \
    $$SOUNDSETTINGS_PATH/alerttonedefaults.h       \
    $$SOUNDSETTINGS_PATH/alerttonebrowserstylable.h \
    $$PUBLIC_HEADERS

SOURCES =                                          \
    ../debug.cpp                                   \
    init.c                                         \
    $$SOUNDSETTINGS_PATH/alerttone.cpp             \
    $$SOUNDSETTINGS_PATH/drilldownitem.cpp         \
    $$SOUNDSETTINGS_PATH/alerttonedefaults.cpp     \
    $$SOUNDSETTINGS_PATH/alerttonetoplevel.cpp     \
    $$SOUNDSETTINGS_PATH/alerttonebrowser.cpp      \
    $$SOUNDSETTINGS_PATH/alerttonebrowsersheet.cpp \
    $$SOUNDSETTINGS_PATH/alerttonebrowserstylable.cpp \
    $$SOUNDSETTINGS_PATH/alerttoneappletmaps.cpp   \
    $$SOUNDSETTINGS_PATH/trackerconnection.cpp     \
    $$SOUNDSETTINGS_PATH/qprofilevalue.cpp         \
    $$SOUNDSETTINGS_PATH/qtrackedvariant.cpp       \
    $$SOUNDSETTINGS_PATH/alerttonedefaultsmodel.cpp \
    $$SOUNDSETTINGS_PATH/soundsettingsutils.cpp    \
    $$SOUNDSETTINGS_PATH/alerttonepreview.cpp      

DESTDIR = lib
target.path += /usr/lib

headers.files = $$PUBLIC_HEADERS
headers.path += $$(DEBIAN_DESTDIR)/usr/include/libmeegocontrol

qtfeature.path = $$[QT_INSTALL_DATA]/mkspecs/features
qtfeature.files = \
    meegocontrol.prf \
    meegocontrol_sound.prf

INSTALLS += \
    qtfeature \
    headers \
    target

