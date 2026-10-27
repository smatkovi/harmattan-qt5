include(../coverage.pri)
include(../../localconfig.pri)
include(../../shared.pri)

TEMPLATE = lib
CONFIG += plugin \
          gui \
          meegotouchcore \
          duicontrolpanel

contains(DEFINES, HAVE_QMSYSTEM) {
    CONFIG += qmsystem2
}

QT += dbus

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

MOC_DIR = .moc
OBJECTS_DIR = .objects

HEADERS = \
    ../debug.h \
    resetbusinesslogic.h \
    resetapplet.h \
    resetwidget.h \
    resetbrief.h 

SOURCES = \
    ../debug.cpp \
    resetbusinesslogic.cpp \
    resetapplet.cpp \
    resetwidget.cpp \
    resetbrief.cpp 

DESTDIR = lib
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

rfs.files = mcpapplets-rfs.sh
rfs.path += $$(DEBIAN_DESTDIR)/$$RFS_DIR

INSTALLS += target \
            rfs \
            desktop

