include(../coverage.pri)
include(../../localconfig.pri)
include(../../shared.pri)

TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouchcore \
          duicontrolpanel \

MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS = \
    ../debug.h \
    testapplet.h \
    testwidget.h \
    testbrief.h 

SOURCES = \
    ../debug.cpp \
    testapplet.cpp \
    testwidget.cpp \
    testbrief.cpp 

DESTDIR = lib
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

INSTALLS += target \
            desktop

