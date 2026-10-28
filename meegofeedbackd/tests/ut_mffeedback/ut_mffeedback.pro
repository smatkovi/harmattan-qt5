include(../common_top.pri)

TARGET = ut_mffeedback

COPY_TESTED_SOURCES = $$FMSRCDIR/lib/mffeedback.cpp $$FMSRCDIR/lib/mfconfig.cpp
copysourcefiles.input = COPY_TESTED_SOURCES
copysourcefiles.output = ${QMAKE_FILE_BASE}.cpp
copysourcefiles.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_BASE}.cpp
copysourcefiles.variable_out = SOURCES
copysourcefiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += copysourcefiles

TEST_SOURCES = $$COPY_TESTED_SOURCES

COPY_TESTED_HEADERS = $$FMSRCDIR/lib/mffeedback.h
copyheaderfiles.input = COPY_TESTED_HEADERS
copyheaderfiles.output = ${QMAKE_FILE_BASE}.h .moc/moc_${QMAKE_FILE_BASE}.cpp
copyheaderfiles.commands = $$QMAKE_COPY ${QMAKE_FILE_IN} ${QMAKE_FILE_BASE}.h
copyheaderfiles.variable_out = HEADERS
copyheaderfiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += copyheaderfiles

MOC_TESTED_HEADERS = $$COPY_TESTED_HEADERS
mocheaderfiles.input = MOC_TESTED_HEADERS
mocheaderfiles.output = moc_${QMAKE_FILE_BASE}.cpp
mocheaderfiles.commands = $$QMAKE_MOC $(DEFINES) $(INCPATH) ${QMAKE_FILE_BASE}.h -o moc_${QMAKE_FILE_BASE}.cpp
mocheaderfiles.variable_out = SOURCES
mocheaderfiles.CONFIG = target_predeps no_link
QMAKE_EXTRA_COMPILERS += mocheaderfiles

#DEFINES += THEME_DIR_PREFIX=\\\"./themes\\\"

SOURCES += ut_mffeedback.cpp \
    mffeedbackhandle_mock.cpp \
    mfmanager_mock.cpp \
    mfbackendfoo.cpp \
    $$FMSRCDIR/lib/mfbackendbase.cpp

HEADERS += ut_mffeedback.h \
    mfmanager.h \
    mfmanager_mock.h \
    mffeedbackhandle.h \
    mffeedbackhandle_mock.h \
    mfbackendfoo.h \
    $$FMSRCDIR/lib/mfbackendinterface.h \
    $$FMSRCDIR/lib/mfbackendbase.h \
    $$FMSRCDIR/lib/mfsourceinterface.h

include(../common_bot.pri)
