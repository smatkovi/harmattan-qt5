include(../coverage.pri)
include(../../localconfig.pri)
include(../../shared.pri)

TEMPLATE      = lib

CONFIG       += plugin \
                gui \
                meegotouchcore \
                duicontrolpanel \

contains(DEFINES, HAVE_QMSYSTEM) {
    CONFIG += qmsystem2
}

QT += gui dbus

MOC_DIR	      = .moc
OBJECTS_DIR   = .objects

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

HEADERS +=  \
    ../debug.h \
    usbapplet.h \
    usbview.h

SOURCES += \
    ../debug.cpp \
    usbapplet.cpp \
    usbview.cpp

DESTDIR = lib
target.path +=  $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

desktop.files += usbapplet.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

INSTALLS += target \
            desktop

