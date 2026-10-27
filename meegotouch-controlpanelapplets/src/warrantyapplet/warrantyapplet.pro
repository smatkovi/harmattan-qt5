include(../coverage.pri)
include(../../localconfig.pri)
include(../../shared.pri)

TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouchcore \
          duicontrolpanel \

contains(DEFINES, HAVE_QMSYSTEM) {
    CONFIG += qmsystem2
}

QT += dbus

MOC_DIR = .moc
OBJECTS_DIR = .objects

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

HEADERS = \
    ../debug.h \
    warrantybusinesslogic.h \
    warrantyapplet.h \
    warrantywidget.h

SOURCES = \
    ../debug.cpp \
    warrantybusinesslogic.cpp \
    warrantyapplet.cpp \
    warrantywidget.cpp

DESTDIR = lib
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

INSTALLS +=   \
    target    \
    desktop

