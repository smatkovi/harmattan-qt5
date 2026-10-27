include(../common_top.pri)

TARGET = ut_meegoimengine_hwr
DEFINES +=
LIBS += ../../src/libmeegoimengine.so.0
LIBS += -L../lib -L../../src
LIBS += -L/usr/lib/meego-imengines/drivers/hwr/ -Wl,-rpath-link=/usr/lib/meego-imengines/drivers/hwr/ -lstubdriver1_hwr

INCLUDEPATH += ../mydriverstub_hwr ../../src ../../includes ../stubbase

# Input
HEADERS += ut_mimengine.h \

SOURCES += ut_mimengine.cpp \

QT = core testlib

include(../common_check.pri)

target.path = /usr/lib/libmeegoimengine-tests/hwr/$$TARGET

