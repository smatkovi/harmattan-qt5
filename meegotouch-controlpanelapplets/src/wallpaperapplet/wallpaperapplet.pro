include(../../localconfig.pri)
include(../coverage.pri)
include(../../shared.pri)

TEMPLATE = lib
MOC_DIR = .moc
OBJECTS_DIR = .objects

QT += dbus

QMAKE_LFLAGS_RPATH = -Wl
QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden -fPIC

CONFIG += plugin \
          gui \
          meegotouchcore \
          quill \
          duicontrolpanel

contains(DEFINES, HAVE_CONTENT_MANAGER) {
    CONFIG += link_pkgconfig
    PKGCONFIG += ContentManager
}

contains(DEFINES, HAVE_QUILL_FILTER) {
    CONFIG += quillimagefilter
}

contains(DEFINES, HAVE_QUILL_METADATA) {
    CONFIG += quillmetadata
}

contains(DEFINES, HAVE_QTSPARQL) {
    CONFIG += qtsparql
}

LIBS += \
    -lthumbnailer

INCLUDEPATH += \
    ../libmeegocontrol \
    /usr/include/thumbnailer 

HEADERS = \
    ../debug.h \
    gridimagewidget.h \
    wallpaperdescriptor.h \
    wallpapercurrentdescriptor.h   \
    wallpapermodel.h               \
    wallpaperlist.h                \
    wallpaperbusinesslogic.h       \
    wallpaperapplet.h              \
    wallpaperwidget.h              \
    wallpaperinfoheader.h          \
    wallpapereditorsheet.h         \
    wallpapereditorwidget.h        \
    wallpaperthumb.h               \
    wallpaperitrans.h

SOURCES = \
    ../debug.cpp \
    gridimagewidget.cpp \
    wallpaperdescriptor.cpp \
    wallpapercurrentdescriptor.cpp \
    wallpaperlist.cpp \
    wallpapermodel.cpp \
    wallpaperbusinesslogic.cpp     \
    wallpaperapplet.cpp            \
    wallpaperwidget.cpp            \
    wallpaperinfoheader.cpp        \
    wallpapereditorsheet.cpp       \
    wallpapereditorwidget.cpp      \
    wallpaperthumb.cpp             \
    wallpaperitrans.cpp

DESTDIR = lib
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

backup.files = wallpaper.conf
backup.path  = /usr/share/backup-framework/applications/

contains(DEFINES, HAVE_CONTENT_MANAGER) {
    CONFIG += link_pkgconfig
    PKGCONFIG += ContentManager
}

desktopsrc = wallpaper.desktop.harmattan
contains(DEFINES, MEEGO) {
    desktopsrc = wallpaper.desktop.meego
}
desktopfile = wallpaper.desktop
desktop.files += $$desktopfile
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel
desktop.commands = cp $$desktopsrc $$desktopfile
desktop.CONFIG += no_check_exist

css.path += $$(DEBIAN_DESTDIR)/usr/share/themes/base/meegotouch/libwallpaperapplet/style
css.files = libwallpaperapplet.css

INSTALLS +=     \
    target      \
    desktop     \
    backup      \
    css

