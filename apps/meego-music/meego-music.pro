TEMPLATE = app
TARGET = meego-music

QT += quick qml concurrent
CONFIG += c++11 link_pkgconfig
PKGCONFIG += gstreamer-1.0 gstreamer-pbutils-1.0 gstreamer-tag-1.0

SOURCES += src/main.cpp src/library.cpp src/player.cpp
HEADERS += src/library.h src/player.h
RESOURCES += meego-music.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-music.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-music.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
