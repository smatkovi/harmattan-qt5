include(../common_top.pri)

#
# FIXME: Temporary thing until the unit tests are set to the new wallpaper
# source.
#
MOC_DIR = ../.wallpaper2moc

SRC_PREFIX = ../../src/wallpaperapplet2
STUB_PREFIX = ../stubs

INCLUDEPATH = \
        $$STUB_PREFIX \
        $$SRC_PREFIX \
        $$INCLUDEPATH

QT += \
        testlib \
        dbus

TEMPLATE = app

DEFINES += \
        UNIT_TEST

TARGET = ut_wallpapergridimagewidget
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
        gui \
        meegotouchcore \
        plugin \
        quill \
        duicontrolpanel \
        link_pkgconfig

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}

PKGCONFIG += ContentManager

HEADERS +=                                       \
    ../../src/debug.h                            \
    ut_wallpapergridimagewidget.h                \
    $$SRC_PREFIX/gridimagewidget.h

SOURCES +=                                       \
    ../../src/debug.cpp                          \
    ut_wallpapergridimagewidget.cpp              \
    $$SRC_PREFIX/gridimagewidget.cpp


INSTALLS += target
