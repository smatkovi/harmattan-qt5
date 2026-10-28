include(../common_top.pri)

TEMPLATE = app
CONFIG += console
TARGET = ft_reconnection
DEFINES += THEME_DIR_PREFIX=\\\"./themes\\\" MF_RECONNECTION_TESTING
INCLUDEPATH += ../../src/daemon ../../src/lib
SRCDIR = ../../src

TEST_SOURCES += \
    $$SRCDIR/lib/mfbackendbase.cpp \
    $$SRCDIR/lib/mffeedback.cpp \
    $$SRCDIR/lib/mffeedbackhandle.cpp \
    $$SRCDIR/lib/mfmanager.cpp \
    $$SRCDIR/lib/mfsession.cpp

SOURCES += \
    main.cpp \
    mfconfig_mock.cpp \
    mfutil_mock.cpp \
    $$SRCDIR/lib/mflog.cpp \
    $$FMSRCDIR/lib/mfsourcebase.cpp \
    ../../meegofeedback-dummy-audio/mfbackenddummyaudio.cpp \
    ../../meegofeedback-dummy-vibra/mfbackenddummyvibra.cpp \
    $$TEST_SOURCES

HEADERS += \
    mfmanagertest.h \
    mfmanagerhandlestest.h \
    $$SRCDIR/lib/mfsourcebase.h \
    $$SRCDIR/lib/mfsourceinterface.h \
    $$SRCDIR/lib/mfbackendbase.h \
    $$SRCDIR/lib/mfbackendinterface.h \
    $$SRCDIR/lib/mfconfig.h \
    $$SRCDIR/lib/mfmanager.h \
    $$SRCDIR/lib/mffeedback.h \
    $$SRCDIR/lib/mffeedbackhandle.h \
    $$SRCDIR/lib/mflog.h \
    $$SRCDIR/lib/mfsession.h \
    $$SRCDIR/lib/mfutil.h \
    ../../meegofeedback-dummy-audio/mfbackenddummyaudio.h \
    ../../meegofeedback-dummy-vibra/mfbackenddummyvibra.h

include(../common_bot.pri)
