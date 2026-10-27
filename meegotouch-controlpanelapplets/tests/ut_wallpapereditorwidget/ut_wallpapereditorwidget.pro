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

TARGET = ut_wallpapereditorwidget
target.path = /usr/lib/$$TEST_PKG_NAME

CONFIG += \
    gui \
    meegotouchcore \
    quill \
    plugin \
    duicontrolpanel

contains(DEFINES, HAVE_GALLERYCORE) {
    CONFIG += gallerycore
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
    CONFIG += qtsparql-tracker-live
}


STYLE_HEADERS +=                                 \
    $$SRC_PREFIX/wallpaperviewwidgetstyle.h     


HEADERS +=                                       \
    $$STUB_PREFIX/thumbnailerstub.h              \
    $$STUB_PREFIX/mdesktopentry.h                \
    $$STUB_PREFIX/filesystemstub.h               \
    $$STUB_PREFIX/qmusbmode.h                    \
    ../../src/debug.h                            \
    ut_wallpapereditorwidget.h                   \
    $$SRC_PREFIX/gridimagewidget.h               \    
    $$SRC_PREFIX/wallpaperlist.h                 \
    $$SRC_PREFIX/wallpapereditorsheet.h          \
    $$SRC_PREFIX/wallpapergconf.h                \
    $$SRC_PREFIX/wallpaperbusinesslogic.h        \
    $$SRC_PREFIX/wallpaperdescriptor.h           \
    $$SRC_PREFIX/wallpaperdescriptor_p.h         \
    $$SRC_PREFIX/wallpaperviewwidget.h           \
    $$SRC_PREFIX/wallpaperitrans.h               \
    $$SRC_PREFIX/wallpapercellcreator.h          \
    $$SRC_PREFIX/wallpaperworkerthread.h         \
    $$SRC_PREFIX/wallpaperviewwidgetstyle.h      \
    $$SRC_PREFIX/wallpaperwidget.h               \
    $$SRC_PREFIX/wallpapermodel.h                \    
    $$SRC_PREFIX/wallpaperutils.h                \
    $$SRC_PREFIX/wallpaperconfiguration.h        \
    $$SRC_PREFIX/wallpaperimagecontentprovider.h \
    $$SRC_PREFIX/wallpaperimagecontentprovider_p.h

SOURCES +=                                       \
    $$STUB_PREFIX/thumbnailerstub.cpp            \
    $$STUB_PREFIX/mdesktopentry.cpp              \
    $$STUB_PREFIX/filesystemstub.cpp             \
    $$STUB_PREFIX/qmusbmode.cpp                  \
    $$STUB_PREFIX/wallpaperbusinesslogic_stub.cpp \
    ../../src/debug.cpp                          \
    ut_wallpapereditorwidget.cpp                 \
    $$SRC_PREFIX/gridimagewidget.cpp             \
    $$SRC_PREFIX/wallpaperlist.cpp               \
    $$SRC_PREFIX/wallpapereditorsheet.cpp        \
    $$SRC_PREFIX/wallpaperdescriptor.cpp         \
    $$SRC_PREFIX/wallpaperdescriptor_p.cpp       \
    $$SRC_PREFIX/wallpaperviewwidget.cpp         \
    $$SRC_PREFIX/wallpaperitrans.cpp             \
    $$SRC_PREFIX/wallpapercellcreator.cpp        \
    $$SRC_PREFIX/wallpaperwidget.cpp             \
    $$SRC_PREFIX/wallpapermodel.cpp              \    
    $$SRC_PREFIX/wallpaperworkerthread.cpp       \
    $$SRC_PREFIX/wallpaperutils.cpp              \
   $$SRC_PREFIX/wallpaperimagecontentprovider.cpp

INSTALLS += target
