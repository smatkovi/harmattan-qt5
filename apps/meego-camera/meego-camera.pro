TEMPLATE = app
TARGET = meego-camera

QT += quick qml concurrent
CONFIG += c++11 link_pkgconfig
PKGCONFIG += gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0

SOURCES += src/main.cpp src/viewfinder.cpp
HEADERS += src/viewfinder.h
RESOURCES += meego-camera.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-camera.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-camera.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
