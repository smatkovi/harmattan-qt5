TEMPLATE = app
TARGET = meego-browser

QT += quick qml
CONFIG += c++11

SOURCES += src/main.cpp src/browserstore.cpp
HEADERS += src/browserstore.h
RESOURCES += meego-browser.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-browser.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-browser.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
