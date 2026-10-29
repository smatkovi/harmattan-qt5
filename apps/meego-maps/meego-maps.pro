TEMPLATE = app
TARGET = meego-maps

QT += quick qml network
CONFIG += c++11

SOURCES += src/main.cpp src/places.cpp
HEADERS += src/places.h
RESOURCES += meego-maps.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-maps.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-maps.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
