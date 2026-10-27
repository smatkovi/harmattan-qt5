
TEMPLATE = lib
LIBNAME = dummyimdriver
TARGET = $${LIBNAME}
LIBS += -L../../src -lmeegoimengine

INCLUDEPATH += ../../src ../../includes

# Input
HEADERS += dummydriver.h
SOURCES += dummydriver.cpp

CONFIG += plugin
QT = core

target.path += /usr/lib/meego-imengines/drivers/words
INSTALLS += target \

