include(../common_top.pri)

TARGET = ut_mfcmdline

TEST_SOURCES = $$FMSRCDIR/daemon/mfcmdline.cpp

SOURCES += ut_mfcmdline.cpp \
    $$TEST_SOURCES

HEADERS += ut_mfcmdline.h \
    $$FMSRCDIR/daemon/mfcmdline.h

include(../common_bot.pri)

