TEMPLATE = app
TARGET = meego-calculator

QT += quick qml
CONFIG += c++11

SOURCES += src/main.cpp
RESOURCES += meego-calculator.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-calculator.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-calculator.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
