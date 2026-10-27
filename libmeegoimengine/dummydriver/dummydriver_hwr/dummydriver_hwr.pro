
TEMPLATE = lib
LIBNAME = dummyimdriver-hwr
TARGET = $${LIBNAME}
LIBS += -L../../src -lmeegoimengine

INCLUDEPATH += ../../src ../../includes

# Input
HEADERS += dummydriver_hwr.h
SOURCES += dummydriver_hwr.cpp

CONFIG += plugin
QT = core

target.path += /usr/lib/meego-imengines/drivers/hwr
INSTALLS += target \

