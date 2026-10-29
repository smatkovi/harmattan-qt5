TEMPLATE = app
TARGET = meego-notes

QT += quick qml
CONFIG += c++11

SOURCES += src/main.cpp src/notestore.cpp
HEADERS += src/notestore.h
RESOURCES += meego-notes.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-notes.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-notes.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
