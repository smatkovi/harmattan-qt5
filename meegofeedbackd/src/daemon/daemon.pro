
include(../../prefix.pri)

TEMPLATE = app
DESTDIR = ../..
TARGET = meegofeedbackd

CONFIG += console \
    link_pkgconfig

QT += network
QT -= gui

QMAKE_CXXFLAGS_RELEASE += -finline-functions
QMAKE_LFLAGS += "-Wl,--as-needed"
# gobject: g_object_unref is called directly (no longer pulled in by GConf)
PKGCONFIG += gconf-2.0 gobject-2.0 contextsubscriber-1.0

INCLUDEPATH += ../lib
LIBS += -lmeegofeedback
QMAKE_LIBDIR += ../..

contains( COV_OPTION, on ) {
    include(../coverage.pri)
}

CONFIG(release, debug|release){
    DEFINES += QT_DEBUG_OUTPUT
    DEFINES += QT_WARNING_OUTPUT
} else {
    DEFINES += MF_DEBUG
}

HEADERS = mfsignalhandler.h \
    mfserver.h \
    mfconnection.h \
    mfconfiguration.h \
    mfconfiguration_p.h \
    mfcmdline.h \
    mfdevicestatelistener.h \
    ../lib/mfcommon.h

SOURCES = mfcmdline.cpp \
    mfsignalhandler.cpp \
    mfserver.cpp \
    mfconnection.cpp \
    mfconfiguration.cpp \
    mfconfiguration_p.cpp \
    mfdevicestatelistener.cpp \
    main.cpp

check.target = check
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check

check-xml.target = check-xml
check-xml.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check-xml

target.path = $$MF_PREFIX/bin
INSTALLS += target
