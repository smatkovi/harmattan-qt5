include(../coverage.pri)
include(../../localconfig.pri)
include(../../shared.pri)

TEMPLATE = lib
CONFIG +=           \
    plugin          \
    gui             \
    meegotouchcore  \
    duicontrolpanel

contains(DEFINES, HAVE_QMSYSTEM) {
    CONFIG += qmsystem2
}

QT += dbus

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

contains(cov, true) {
    QMAKE_CXXFLAGS += --coverage
    QMAKE_LFLAGS += --coverage
}

MOC_DIR = .moc
OBJECTS_DIR = .objects
HEADERS =                  \
    ../debug.h             \
    batteryapplet.h        \
    batterywidget.h        \
    batterybusinesslogic.h \
    percentagecontainer.h  \
    slidercontainer.h      \
    dcpbattery.h           \
    batteryimage.h

SOURCES =                    \
    ../debug.cpp             \
    batteryapplet.cpp        \
    batterywidget.cpp        \
    batterybusinesslogic.cpp \
    percentagecontainer.cpp  \
    slidercontainer.cpp      \
    batteryimage.cpp

DESTDIR = lib
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

css.files = libbatteryapplet.css
css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libbatteryapplet/style

desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

INSTALLS += target \
            css \
            desktop

