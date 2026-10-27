include(../common_top.pri)

TARGET = ut_meegoimengine_words
DEFINES += ET9_ALPHABETIC_MODULE
LIBS += ../../src/libmeegoimengine.so
LIBS += -L../lib -L../../src
LIBS += -L/usr/lib/meego-imengines/drivers/words -Wl,-rpath-link=/usr/lib/meego-imengines/drivers/words -lstubdriver1_words


INCLUDEPATH += ../mydriverstub_words ../../src ../../includes ../stubbase

# stub
HEADERS += ../stubbase/stubbase.h
SOURCES += ../stubbase/stubbase.cpp

# Input
HEADERS += ut_mimengine.h \

SOURCES += ut_mimengine.cpp \

QT = core testlib

include(../common_check.pri)

target.path = /usr/lib/libmeegoimengine-tests/words/$$TARGET

