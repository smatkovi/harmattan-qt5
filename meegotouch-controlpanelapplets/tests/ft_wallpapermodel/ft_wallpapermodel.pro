include(../common_top.pri)

SRC_PREFIX = ../../src/wallpaperapplet
MC_PREFIX = ../../src/libmeegocontrol
STUB_PREFIX = ../stubs

INCLUDEPATH += \ 
    $$SRC_PREFIX 

QT += \
    testlib \
    dbus 

LIBS += \
    -lthumbnailer

INCLUDEPATH += \
    /usr/include/thumbnailer \
    $$MC_PREFIX

TEMPLATE = app
DEFINES += \
	UNIT_TEST \
	FUNCTIONAL_TEST

TARGET = ft_wallpapermodel
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
	plugin \
	gui \
    quill \
	meegotouchcore \
	duicontrolpanel \
    link_pkgconfig

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

PKGCONFIG += ContentManager


HEADERS += \
    ../../src/debug.h \
    ft_wallpapermodel.h \
    $$SRC_PREFIX/gridimagewidget.h \
    $$SRC_PREFIX/wallpapergconf.h \
    $$SRC_PREFIX/wallpaperbusinesslogic.h \
    $$SRC_PREFIX/wallpaperdescriptor.h \
    $$SRC_PREFIX/wallpapercurrentdescriptor.h \
    $$SRC_PREFIX/wallpaperlist.h \
    $$SRC_PREFIX/wallpapermodel.h \
    $$SRC_PREFIX/wallpaperitrans.h \
    $$SRC_PREFIX/wallpaperthumb.h

SOURCES += \
    ../../src/debug.cpp \
    ft_wallpapermodel.cpp \
    $$SRC_PREFIX/gridimagewidget.cpp \
    $$SRC_PREFIX/wallpaperbusinesslogic.cpp \
    $$SRC_PREFIX/wallpaperdescriptor.cpp \
    $$SRC_PREFIX/wallpapercurrentdescriptor.cpp \
    $$SRC_PREFIX/wallpaperlist.cpp \
    $$SRC_PREFIX/wallpapermodel.cpp \
    $$SRC_PREFIX/wallpaperitrans.cpp \
    $$SRC_PREFIX/wallpaperthumb.cpp

INSTALLS += target
