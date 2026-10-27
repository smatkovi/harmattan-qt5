include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

TEMPLATE = lib
MOC_DIR = .moc
OBJECTS_DIR = .objects

QT += dbus

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

CONFIG +=             \
    m                 \
    plugin            \
    gui               \
    meegotouchcore    \
    duicontrolpanel

contains(DEFINES, HAVE_GALLERYCORE) {
    CONFIG += gallerycore
}

contains(DEFINES, HAVE_QMSYSTEM) {
    CONFIG += qmsystem2
}

contains(DEFINES, HAVE_CONTENT_MANAGER) {
    CONFIG += link_pkgconfig
    PKGCONFIG += ContentManager
}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
    CONFIG += qtsparql-tracker-live
}

LIBS += \
    -lthumbnailer

INCLUDEPATH += \
    ../libmeegocontrol \
    /usr/include/thumbnailer 

STYLE_HEADERS +=                   \
    wallpaperviewwidgetstyle.h     

HEADERS =                          \
    ../debug.h                     \
    wallpaperconfiguration.h       \
    wallpaperutils.h               \
    wallpaperworkerthread.h        \
    gridimagewidget.h              \
    wallpaperdescriptor_p.h        \
    wallpaperdescriptor.h          \
    wallpapercellcreator.h          \
    wallpapermodel.h               \
    wallpaperlist.h                \
    wallpaperbusinesslogic.h       \
    wallpaperapplet.h              \
    wallpaperwidget.h              \
    wallpaperlistsheet.h           \
    wallpapereditorsheet.h         \
    wallpaperviewwidgetstyle.h     \
    wallpaperviewwidget.h          \
    wallpaperitrans.h              \
    wallpaperimagecontentprovider_p.h \
    wallpaperimagecontentprovider.h

SOURCES =                          \
    ../debug.cpp                   \
    wallpaperconfiguration.cpp     \
    wallpaperutils.cpp             \
    wallpaperworkerthread.cpp      \
    gridimagewidget.cpp            \
    wallpaperdescriptor_p.cpp      \
    wallpaperdescriptor.cpp        \
    wallpaperlist.cpp              \
    wallpapercellcreator.cpp       \
    wallpapermodel.cpp             \
    wallpaperbusinesslogic.cpp     \
    wallpaperapplet.cpp            \
    wallpaperwidget.cpp            \
    wallpaperlistsheet.cpp         \
    wallpapereditorsheet.cpp       \
    wallpaperviewwidget.cpp        \
    wallpaperitrans.cpp            \
    wallpaperimagecontentprovider.cpp

DESTDIR = lib
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

backup.files = wallpaper.conf
backup.path  = /usr/share/backup-framework/applications/

desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel

css.path += \
    $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libwallpaperapplet2/style
css.files = libwallpaperapplet2.css

dupfinder.path += $$(DEBIAN_DESTDIR)/usr/bin
dupfinder.files += wallpaper_duplicate_finder.sh

INSTALLS +=     \
    target      \
    desktop     \
    backup      \
    dupfinder   \
    css

