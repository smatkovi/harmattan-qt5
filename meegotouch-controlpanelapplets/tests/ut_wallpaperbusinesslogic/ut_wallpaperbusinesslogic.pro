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
DEFINES += UNIT_TEST
TARGET = ut_wallpaperbusinesslogic
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG +=            \
    link_pkgconfig   \
    plugin           \
    gui              \
    quill            \
    meegotouchcore   \
    duicontrolpanel

PKGCONFIG += gconf-2.0

contains(DEFINES, HAVE_GALLERYCORE) {
    CONFIG += gallerycore
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
    CONFIG += qtsparql-tracker-live
}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}


HEADERS +=                                       \
    $$STUB_PREFIX/thumbnailerstub.h              \
    $$STUB_PREFIX/mdesktopentry.h                \
    $$STUB_PREFIX/filesystemstub.h               \
    ../../src/debug.h                            \
    ut_wallpaperbusinesslogic.h                  \
    $$SRC_PREFIX/wallpapergconf.h                \
    $$SRC_PREFIX/wallpaperbusinesslogic.h        \
    $$SRC_PREFIX/wallpaperdescriptor.h           \
    $$SRC_PREFIX/wallpaperdescriptor_p.h         \
    $$SRC_PREFIX/wallpaperitrans.h               \
    $$SRC_PREFIX/wallpaperworkerthread.h         \
    $$SRC_PREFIX/wallpaperutils.h                \
    $$SRC_PREFIX/wallpaperconfiguration.h        \
    $$SRC_PREFIX/wallpaperimagecontentprovider.h \
    $$SRC_PREFIX/wallpaperimagecontentprovider_p.h

SOURCES +=                                       \
    $$STUB_PREFIX/thumbnailerstub.cpp            \
    $$STUB_PREFIX/mdesktopentry.cpp              \
    $$STUB_PREFIX/filesystemstub.cpp             \
    $$STUB_PREFIX/wallpaperutils_stub.cpp        \
    ../../src/debug.cpp                          \
    ut_wallpaperbusinesslogic.cpp                \
    $$SRC_PREFIX/wallpaperbusinesslogic.cpp      \
    $$SRC_PREFIX/wallpaperdescriptor.cpp         \
    $$SRC_PREFIX/wallpaperdescriptor_p.cpp       \
    $$SRC_PREFIX/wallpaperworkerthread.cpp       \
    $$SRC_PREFIX/wallpaperitrans.cpp             \
    $$SRC_PREFIX/wallpaperimagecontentprovider.cpp

INSTALLS += target
