include(../common_top.pri)

TEMPLATE = app
CONFIG += QtTest meegotouch meegoimframework
LIBS += -L/usr/lib/ -Wl,-rpath=/usr/lib/meego-im-plugins/ -lmeego-keyboard

include(../common_check.pri)

HEADERS += \
    ut_enginehandlertonal.h \
    ../stubs/minputmethodhoststub.h \

SOURCES += \
    ut_enginehandlertonal.cpp \
    ../stubs/minputmethodhoststub.cpp \

