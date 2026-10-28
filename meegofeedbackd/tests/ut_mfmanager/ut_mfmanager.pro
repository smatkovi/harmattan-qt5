include(../common_top.pri)

TARGET = ut_mfmanager

DEFINES += THEME_DIR_PREFIX=\\\"./themes\\\"

TEST_SOURCES = $$FMSRCDIR/lib/mfmanager.cpp

SOURCES += ut_mfmanager.cpp \
    mfsourcefoo.cpp \
    mfbackendfoo.cpp \
    globals.cpp \
    qdir_mock.cpp \
    qpluginloader_mock.cpp \
    $$FMSRCDIR/lib/mfbackendbase.cpp \
    $$FMSRCDIR/lib/mfsourcebase.cpp \
    $$FMSRCDIR/lib/mfconfig.cpp \
    $$FMSRCDIR/lib/mffeedback.cpp \
    $$FMSRCDIR/lib/mffeedbackhandle.cpp \
    $$FMSRCDIR/lib/mflog.cpp \
    $$TEST_SOURCES

HEADERS += mfbackendfoo.h \
    mfsourcefoo.h \
    globals.h \
    qpluginloader_mock.h \
    ut_mfmanager.h \
    $$FMSRCDIR/lib/mfbackendbase.h \
    $$FMSRCDIR/lib/mfbackendinterface.h \
    $$FMSRCDIR/lib/mfsourcebase.h \
    $$FMSRCDIR/lib/mfsourceinterface.h \
    $$FMSRCDIR/lib/mfconfig.h \
    $$FMSRCDIR/lib/mffeedback.h \
    $$FMSRCDIR/lib/mffeedbackhandle.h \
    $$FMSRCDIR/lib/mflog.h \
    $$FMSRCDIR/lib/mfcommon.h \
    $$FMSRCDIR/lib/mfmanager.h

include(../common_bot.pri)
