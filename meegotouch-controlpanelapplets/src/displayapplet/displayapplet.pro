include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouchcore \
          duicontrolpanel \
          qmsystem2

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

QT += dbus gui

MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS = \
    ../debug.h \
    displayapplet.h \
    displaywidget.h \
    brightnessbrief.h \
    displaybusinesslogic.h

SOURCES = \
    ../debug.cpp \
    displayapplet.cpp \
    displaywidget.cpp \
    brightnessbrief.cpp \
    displaybusinesslogic.cpp

DESTDIR = lib
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

css.files = libdisplayapplet.css
css.path = $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libdisplayapplet/style

INSTALLS += target \
            desktop \
            css

