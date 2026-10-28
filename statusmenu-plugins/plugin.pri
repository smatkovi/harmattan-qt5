# what every status menu plugin here builds and installs with: a plugin of
# libmeegotouch's application extensions (MApplicationExtensionArea loads it
# by its desktop entry)
TEMPLATE = lib
INCLUDEPATH += $$PWD
HEADERS += $$PWD/invertedlistitem.h
# M_APPLICATION_EXTENSION_DIR and _DATA_DIR
load(meegotouch_defines)
CONFIG += plugin meegotouch system-ui link_pkgconfig
QT += widgets dbus
MOC_DIR = .moc
OBJECTS_DIR = .obj
TARGET = $$qtLibraryTarget(statusindicatormenu-$$PLUGIN)
target.path = $$M_APPLICATION_EXTENSION_DIR
desktop.files = statusindicatormenu-$${PLUGIN}.desktop
desktop.path = $$M_APPLICATION_EXTENSION_DATA_DIR
INSTALLS += target desktop
