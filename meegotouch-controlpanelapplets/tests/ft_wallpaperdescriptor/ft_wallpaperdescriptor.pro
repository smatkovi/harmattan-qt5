include(../common_top.pri)

SRC_PREFIX = ../../src/wallpaperapplet2
MC_PREFIX = ../../src/libmeegocontrol
STUB_PREFIX = ../stubs

MOC_DIR = .moc

INCLUDEPATH += \
    $$SRC_PREFIX \
    $$MC_PREFIX

QT += \
    testlib \
    dbus

TEMPLATE = app
DEFINES += \
	UNIT_TEST \
	FUNCTIONAL_TEST

TARGET = ft_wallpaperdescriptor
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
	gui \
    quill \
	meegotouchcore

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}

HEADERS += \
    ../../src/debug.h \
    ft_wallpaperdescriptor.h \
    $$SRC_PREFIX/wallpaperdescriptor.h \
    $$SRC_PREFIX/wallpaperdescriptor_p.h \
    $$SRC_PREFIX/wallpaperitrans.h \
    $$SRC_PREFIX/wallpaperconfiguration.h \
    $$SRC_PREFIX/wallpaperutils.h

SOURCES += \
    ../../src/debug.cpp \
    ft_wallpaperdescriptor.cpp \
    $$SRC_PREFIX/wallpaperdescriptor.cpp \
    $$SRC_PREFIX/wallpaperdescriptor_p.cpp \
    $$SRC_PREFIX/wallpaperitrans.cpp \
    $$SRC_PREFIX/wallpaperutils.cpp

INSTALLS += target
