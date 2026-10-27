include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouchcore \
          mobility \
          duicontrolpanel \
          link_pkgconfig \
          qdbus \
          ofono-qt

QT += dbus

MOBILITY += systeminfo

PKG_CONFIG+=libofono-qt
LIBS+=-lofono-qt
MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS = \
    ../debug.h \
    gprsbusinesslogic.h \
    gprsapplet.h \
    gprswidget.h \
    gprsbrief.h

SOURCES = \
    ../debug.cpp \
    gprsbusinesslogic.cpp \
    gprsapplet.cpp \
    gprswidget.cpp \
    gprsbrief.cpp

DESTDIR = lib
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

configfile.files = operators
configfile.path = $$(DEBIAN_DESTDIR)/etc/meegotouch/connections/gprs

css.files = libgprsapplet.css
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libgprsapplet/style

desktop.files += *.desktop
desktop.path = /usr/lib/duicontrolpanel

message("The plugin will be installed to: " $$target.path)
INSTALLS += target \
            desktop \
            css \
            configfile
