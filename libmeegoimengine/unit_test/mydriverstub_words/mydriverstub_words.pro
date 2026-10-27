include(../common_top.pri)

TEMPLATE = lib
LIBNAME = stubdriver1_words
TARGET = ../lib/$${LIBNAME}
DEFINES += ET9_ALPHABETIC_MODULE
LIBS += -L../../src -lmeegoimengine

INCLUDEPATH += ../../src ../../includes ../stubbase

# stub
HEADERS += ../stubbase/stubbase.h
SOURCES += ../stubbase/stubbase.cpp

# Input
HEADERS += stubdriver.h
SOURCES += stubdriver.cpp

CONFIG += plugin
QT = core

target.path += /usr/lib/meego-imengines/drivers/words
INSTALLS += target \

QMAKE_CLEAN += ./lib/*

QMAKE_EXTRA_TARGETS += check
check.target = check
check.command = $$system(true)
check.depends = ../lib/lib$${LIBNAME}.so

QMAKE_EXTRA_TARGETS += memcheck
memcheck.target = memcheck
memcheck.command = $$system(true)
memcheck.depends = ../lib/lib$${LIBNAME}.so
