TEMPLATE = app
TARGET = soundsettings
DEPENDPATH += .
INCLUDEPATH += .

#
# FIXME: We assume that we found the content manager.
#
DEFINES += \
    HAVE_CONTENT_MANAGER

contains(DEFINES, HAVE_CONTENT_MANAGER) {
    CONFIG += link_pkgconfig
    PKGCONFIG += ContentManager
}

CONFIG +=           \
    meegotouchcore  \
    duicontrolpanel \
    meegocontrol    \
    qt              \
    silent

QT += gui

HEADERS += soundsettings.h
SOURCES += soundsettings.cpp

