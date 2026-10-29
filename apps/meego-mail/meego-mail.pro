TEMPLATE = app
TARGET = meego-mail

QT += quick qml network
CONFIG += c++11

SOURCES += src/main.cpp src/mime.cpp src/imap.cpp src/smtp.cpp src/mailstore.cpp
HEADERS += src/mime.h src/imap.h src/smtp.h src/mailstore.h
RESOURCES += meego-mail.qrc

isEmpty(PREFIX): PREFIX = /usr

target.path = $$PREFIX/bin
desktop.files = data/meego-mail.desktop
desktop.path = $$PREFIX/share/applications
hildon.files = data/hildon/meego-mail.desktop
hildon.path = $$PREFIX/share/applications/hildon
INSTALLS += target desktop hildon
