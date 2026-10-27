include(../coverage.pri)
include(../../localconfig.pri)
include(../../shared.pri)

TEMPLATE = lib

load(meegotouch_defines)

DEFINES += M_APPLICATION_EXTENSION_DIR=\\\"\"$$M_APPLICATION_EXTENSION_DIR\\\"\"

include($$[QT_INSTALL_DATA]/mkspecs/dcpconfig.pri)

CONFIG +=           \
    plugin          \
    gui             \
    meegotouchcore  \
    link_pkgconfig  \
    duicontrolpanel \
    timed           \
    mobility

MOBILITY +=         \
    systeminfo

PKGCONFIG += dbus-1 profile gstreamer-0.10

DEFINES += MC_EXPORT=""
INCLUDEPATH += \
    ../libmeegocontrol

contains(DEFINES, HAVE_LIBRESOURCEQT) {
    PKGCONFIG += libresourceqt1
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

contains(DEFINES, HAVE_CONTENT_MANAGER) {
    CONFIG += contentmanager
}

QT += dbus

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS =                      \
    ../debug.h                 \
    drilldownitem.h            \
    trackerconnection.h        \
    profilebackend.h           \
    profiledatainterface.h     \
    alerttone.h                \
    alerttoneappletmaps.h      \
    profilecontainer.h         \
    alerttoneappletwidget.h    \
    alerttonebrowser.h         \
    alerttonebrowsersheet.h    \
    alerttonedefaults.h        \
    alerttonedefaultsmodel.h   \
    alerttonepreview.h         \
    alerttonetoplevel.h        \
    alerttonewidget.h          \
    gconfstringcombo.h         \
    profileintcombo.h          \
    qprofilevalue.h            \
    qtrackedvariant.h          \
    soundsettingsapplet.h      \
    soundsettingsutils.h       \
    static.h                   \
    alerttonebrowserstylable.h \
    profilewidgetcontainer.h   \
    $$(NULL)

SOURCES =                      \
    ../debug.cpp               \
    drilldownitem.cpp          \
    trackerconnection.cpp      \
    profilebackend.cpp         \
    profiledatainterface.cpp   \
    alerttone.cpp              \
    alerttoneappletmaps.cpp    \
    profilecontainer.cpp       \
    alerttoneappletwidget.cpp  \
    alerttonebrowser.cpp       \
    alerttonebrowsersheet.cpp  \
    alerttonedefaults.cpp      \
    alerttonedefaultsmodel.cpp \
    alerttonepreview.cpp       \
    alerttonetoplevel.cpp      \
    alerttonewidget.cpp        \
    gconfstringcombo.cpp       \
    profileintcombo.cpp        \
    qprofilevalue.cpp          \
    qtrackedvariant.cpp        \
    soundsettingsutils.cpp     \
    soundsettingsapplet.cpp    \
    alerttonebrowserstylable.cpp \
    profilewidgetcontainer.cpp \
    $$(NULL)

DESTDIR = lib

css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libsoundsettingsapplet/style
css.files = libsoundsettingsapplet.css

# XXX: HACK:
# This file will be added as an import to duicontrolpanels css file in postinst script
harmattancss.path += $$(DEBIAN_DESTDIR)/usr/share/themes/blanco/meegotouch/duicontrolpanel/style
harmattancss.files += soundsettingsapplet.css

desktopsrc = soundsettingsapplet.desktop.harmattan
contains(DEFINES, MEEGO) {
    desktopsrc = soundsettingsapplet.desktop.meego
}
desktopfile = soundsettingsapplet.desktop
desktop.files += $$desktopfile
desktop.path = $$(DEBIAN_DESTDIR)/$$DCP_DESKTOP_DIR
desktop.commands = cp -f $$desktopsrc $$desktopfile
desktop.CONFIG += no_check_exist

target.path += $$(DEBIAN_DESTDIR)/$$DCP_APPLET_DIR

INSTALLS += \
    target  \
    desktop \
    css \
    harmattancss

