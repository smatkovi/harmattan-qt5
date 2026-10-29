TEMPLATE = app
TARGET = meego-contacts

QT += quick qml
CONFIG += c++11

SOURCES += src/main.cpp src/contactstore.cpp src/vcard.cpp
HEADERS += src/contactstore.h src/vcard.h
RESOURCES += meego-contacts.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-contacts.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-contacts.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
