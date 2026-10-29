TEMPLATE = app
TARGET = meego-messages

QT += quick qml dbus sql
CONFIG += c++11

SOURCES += src/main.cpp src/sms.cpp
HEADERS += src/sms.h
RESOURCES += meego-messages.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-messages.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-messages.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
