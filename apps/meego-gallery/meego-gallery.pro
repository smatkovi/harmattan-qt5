TEMPLATE = app
TARGET = meego-gallery

QT += quick qml concurrent
CONFIG += c++11

SOURCES += src/main.cpp src/pictures.cpp
HEADERS += src/pictures.h
RESOURCES += meego-gallery.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-gallery.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-gallery.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
