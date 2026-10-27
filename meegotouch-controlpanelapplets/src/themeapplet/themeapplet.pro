include(../coverage.pri)
include(../../localconfig.pri)
include(../../shared.pri)

TEMPLATE = lib
CONFIG += plugin \
          gui \
          dbus \
          meegotouchcore \
          meegotouch_defines \
          duicontrolpanel \

MOC_DIR = .moc
OBJECTS_DIR = .objects

QT += dbus

load(meegotouch_defines)
DEFINES += \
    MEEGO_THEMEDIR=\\\"\"$$M_THEME_DIR\"\\\"

HEADERS = \
    ../debug.h \
    drilldownitem.h \
    themedescriptor.h \
    themebusinesslogic.h \
    themeapplet.h \
    themelistmodel.h \
    themewidget.h \
    themebrief.h \
    themecellcreator.h \
    themedialog.h

SOURCES = \
    ../debug.cpp \
    drilldownitem.cpp \
    themedescriptor.cpp \
    themebusinesslogic.cpp \
    themeapplet.cpp \
    themelistmodel.cpp \
    themewidget.cpp \
    themebrief.cpp \
    themecellcreator.cpp \
    themedialog.cpp

DESTDIR = lib
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

css.files = libthemeapplet.css
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libthemeapplet/style

INSTALLS += target \
            css \
            desktop 

