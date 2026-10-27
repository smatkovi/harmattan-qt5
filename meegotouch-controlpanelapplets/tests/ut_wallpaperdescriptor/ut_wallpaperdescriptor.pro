include(../common_top.pri)

#
# FIXME: Temporary thing until the unit tests are set to the new wallpaper
# source.
#
MOC_DIR = ../.wallpaper2moc

SRC_PREFIX = ../../src/wallpaperapplet2
STUB_PREFIX = ../stubs

INCLUDEPATH +=         \
	$$SRC_PREFIX       \
    $$STUB_PREFIX

QT +=                  \
    testlib            \
    dbus 

TEMPLATE = app
DEFINES += UNIT_TEST
TARGET = ut_wallpaperdescriptor
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG +=             \
	plugin            \
	gui               \
	meegotouchcore    \
	duicontrolpanel

#contains(DEFINES, HAVE_QMSYSTEM) {
    CONFIG += qmsystem2
#}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

HEADERS +=                                  \
    ../../src/debug.h                       \
    ut_wallpaperdescriptor.h                \
    $$SRC_PREFIX/wallpaperutils.h           \
    $$SRC_PREFIX/wallpaperdescriptor.h      \
    $$SRC_PREFIX/wallpaperdescriptor_p.h 

SOURCES +=                                  \
    ../../src/debug.cpp                     \
    ut_wallpaperdescriptor.cpp              \
    $$SRC_PREFIX/wallpaperutils.cpp         \
    $$SRC_PREFIX/wallpaperdescriptor.cpp    \
    $$SRC_PREFIX/wallpaperdescriptor_p.cpp

INSTALLS += target


