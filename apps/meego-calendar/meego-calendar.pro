TEMPLATE = app
TARGET = meego-calendar

QT += quick qml sql
CONFIG += c++11

SOURCES += src/main.cpp src/eventstore.cpp
HEADERS += src/eventstore.h
RESOURCES += meego-calendar.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-calendar.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-calendar.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
