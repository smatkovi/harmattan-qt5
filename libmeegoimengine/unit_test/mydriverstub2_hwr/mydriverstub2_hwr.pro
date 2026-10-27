include(../common_top.pri)

TEMPLATE = lib
LIBNAME = stubdriver2_hwr
TARGET = ../lib/$${LIBNAME}
DEFINES +=
LIBS += -L../../src -lmeegoimengine

INCLUDEPATH += ../../src ../../includes ../stubbase

# stub
HEADERS += ../stubbase/stubbase.h
SOURCES += ../stubbase/stubbase.cpp

# Input
HEADERS += stubdriver2.h
SOURCES += stubdriver2.cpp

CONFIG += plugin
QT = core

target.path += /usr/lib/meego-imengines/drivers/hwr
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
