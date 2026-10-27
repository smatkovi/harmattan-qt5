TEMPLATE = app
TARGET = test
DEPENDPATH += .
INCLUDEPATH += .

CONFIG +=           \
    meegotouchcore  \
    qt              \
    silent

QT += gui

HEADERS += fullscreen-test.h
SOURCES += fullscreen-test.cpp

