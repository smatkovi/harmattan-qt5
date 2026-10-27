include(../common_top.pri)
include(../../localconfig.pri)

SRC_PREFIX = ../../src/wallpaperapplet2
STUB_PREFIX = ../stubs
MOC_DIR = ../.wallpaper2moc

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

TARGET = ut_wallpaperlist
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui \
    meegotouchcore \
    quill \
    duicontrolpanel

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}

contains(DEFINES, HAVE_GALLERYCORE) {
    CONFIG += gallerycore
}

HEADERS += \
    $$STUB_PREFIX/thumbnailerstub.h \
    $$STUB_PREFIX/mdesktopentry.h \
    $$STUB_PREFIX/filesystemstub.h \
    $$STUB_PREFIX/qmusbmode.h \
    ../../src/debug.h \
    ut_wallpaperlist.h \
    $$SRC_PREFIX/wallpaperlist.h \
    $$SRC_PREFIX/wallpaperconfiguration.h \
    $$SRC_PREFIX/wallpapercellcreator.h \
    $$SRC_PREFIX/gridimagewidget.h \
    $$SRC_PREFIX/wallpapergconf.h \
    $$SRC_PREFIX/wallpaperbusinesslogic.h \
    $$SRC_PREFIX/wallpaperdescriptor.h \
    $$SRC_PREFIX/wallpaperdescriptor_p.h \
    $$SRC_PREFIX/wallpaperitrans.h \
    $$SRC_PREFIX/wallpaperutils.h \
    $$SRC_PREFIX/wallpapermodel.h

SOURCES += \
    $$STUB_PREFIX/thumbnailerstub.cpp \
    $$STUB_PREFIX/mdesktopentry.cpp \
    $$STUB_PREFIX/filesystemstub.cpp \
    $$STUB_PREFIX/qmusbmode.cpp \
    ../../src/debug.cpp \
    ut_wallpaperlist.cpp \
    $$SRC_PREFIX/wallpaperlist.cpp \
    $$SRC_PREFIX/wallpaperconfiguration.cpp \
    $$SRC_PREFIX/wallpapercellcreator.cpp \
    $$SRC_PREFIX/gridimagewidget.cpp \
    $$STUB_PREFIX/wallpaperbusinesslogic_stub.cpp \
    $$STUB_PREFIX/wallpapermodel_stub.cpp \
    $$SRC_PREFIX/wallpaperdescriptor.cpp \
    $$SRC_PREFIX/wallpaperdescriptor_p.cpp \
    $$SRC_PREFIX/wallpaperitrans.cpp \
    $$SRC_PREFIX/wallpaperutils.cpp

INSTALLS += target

