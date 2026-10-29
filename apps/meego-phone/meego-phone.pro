TEMPLATE = app
TARGET = meego-phone

QT += quick qml dbus
CONFIG += c++11

SOURCES += src/main.cpp src/modem.cpp src/callhistory.cpp src/audioroute.cpp
HEADERS += src/modem.h src/callhistory.h src/audioroute.h
RESOURCES += meego-phone.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-phone.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-phone.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
