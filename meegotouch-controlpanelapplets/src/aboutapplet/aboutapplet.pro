include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

SUBDIRS += translations

TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouchcore \
          mobility \
          duicontrolpanel \
          link_pkgconfig

QT += dbus
MOBILITY += systeminfo

contains(DEFINES, HAVE_CONTENT_ACTION) {
    PKGCONFIG += contentaction-0.1
}

MOC_DIR = .moc
OBJECTS_DIR = .objects

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

HEADERS =                   \
    ../debug.h              \
    logowidget.h            \
    aboutbusinesslogic.h    \
    aboutapplet.h           \
    aboutwidget.h

SOURCES =                   \
    ../debug.cpp            \
    logowidget.cpp          \
    aboutbusinesslogic.cpp  \
    aboutapplet.cpp         \
    aboutwidget.cpp

DESTDIR = lib
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

css.files = libaboutapplet.css
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libaboutapplet/style

target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

INSTALLS += target  \
            desktop \
            css

