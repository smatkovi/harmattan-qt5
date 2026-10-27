include(../common_top.pri)

SRC_PREFIX = ../../src/soundsettingsapplet
STUB_PREFIX = ../stubs

INCLUDEPATH += $$SRC_PREFIX \
               $$STUB_PREFIX


SOURCES +=  ut_soundsettingsutils.cpp \
            $$SRC_PREFIX/soundsettingsutils.cpp \
	    ../../src/debug.cpp  \
            $$SRC_PREFIX/trackerconnection.cpp          \
            $$SRC_PREFIX/alerttone.cpp                  \
            $$SRC_PREFIX/qprofilevalue.cpp              \
            $$SRC_PREFIX/qtrackedvariant.cpp           \
            $$SRC_PREFIX/alerttonepreview.cpp 
	    



HEADERS +=  ut_soundsettingsutils.h \
            $$SRC_PREFIX/soundsettingsutils.h \
	    ../../src/debug.h  \
            $$SRC_PREFIX/trackerconnection.h          \
            $$SRC_PREFIX/alerttone.h                  \
            $$SRC_PREFIX/qprofilevalue.h            \
            $$SRC_PREFIX/qtrackedvariant.h \
            $$SRC_PREFIX/alerttonepreview.h  

	    


QT += testlib \
      xml \
      dbus 


PKGCONFIG += dbus-1 profile ContentManager gstreamer-0.10 libresourceqt1

CONFIG += \
    gui             \
    plugin          \
    meegotouchcore  \
    debug           \
    link_pkgconfig  \
    duicontrolpanel \
    timed           \
    qtsparql



TEMPLATE = app
DEFINES += UNIT_TEST



TARGET = ut_soundsettingsutils

target.path = /usr/lib/$$TEST_PKG_NAME

INSTALLS += target
