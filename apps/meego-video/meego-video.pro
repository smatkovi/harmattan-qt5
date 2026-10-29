TEMPLATE = app
TARGET = meego-video

QT += quick qml concurrent
CONFIG += c++11 link_pkgconfig
PKGCONFIG += gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0 gstreamer-pbutils-1.0

SOURCES += src/main.cpp src/videoitem.cpp src/videolibrary.cpp
HEADERS += src/videoitem.h src/videolibrary.h
RESOURCES += meego-video.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-video.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-video.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
