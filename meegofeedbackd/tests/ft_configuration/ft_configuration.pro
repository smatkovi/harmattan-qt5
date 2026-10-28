include(../common_top.pri)

TEMPLATE = app
CONFIG += console link_pkgconfig
TARGET = ft_configuration
DEFINES += THEME_DIR_PREFIX=\\\"./themes\\\" MF_THEME_TESTING
INCLUDEPATH += ../../src/daemon ../../src/lib
SRCDIR = ../../src
PKGCONFIG += gobject-2.0

SOURCES += \
    main.cpp \
    gconf_mock.cpp \
    mfutil_mock.cpp \
    globals.cpp \
    $$SRCDIR/lib/mfsourcebase.cpp \
    $$SRCDIR/lib/mfbackendbase.cpp \
    $$SRCDIR/lib/mflog.cpp \
    $$SRCDIR/lib/mfconfig.cpp \
    $$SRCDIR/daemon/mfconfiguration.cpp \
    $$SRCDIR/daemon/mfconfiguration_p.cpp \
    $$SRCDIR/lib/mffeedback.cpp \
    $$SRCDIR/lib/mffeedbackhandle.cpp \
    $$SRCDIR/lib/mfmanager.cpp \
    $$SRCDIR/lib/mfsession.cpp \
    ../../meegofeedback-dummy-audio/mfbackenddummyaudio.cpp \
    ../../meegofeedback-dummy-vibra/mfbackenddummyvibra.cpp

HEADERS += \
    mfmanagertest.h \
    mffeedbacktest.h \
    gconf/gconf-client.h \
    gconf_mock.h \
    globals.h \
    $$SRCDIR/lib/mfsourcebase.h \
    $$SRCDIR/lib/mfsourceinterface.h \
    $$SRCDIR/lib/mfbackendbase.h \
    $$SRCDIR/lib/mfbackendinterface.h \
    $$SRCDIR/lib/mfconfig.h \
    $$SRCDIR/lib/mfmanager.h \
    $$SRCDIR/lib/mffeedback.h \
    $$SRCDIR/lib/mffeedbackhandle.h \
    $$SRCDIR/lib/mflog.h \
    $$SRCDIR/lib/mfcommon.h \
    $$SRCDIR/lib/mfsession.h \
    $$SRCDIR/daemon/mfconfiguration.h \
    $$SRCDIR/daemon/mfconfiguration_p.h \
    ../../meegofeedback-dummy-audio/mfbackenddummyaudio.h \
    ../../meegofeedback-dummy-vibra/mfbackenddummyvibra.h

support_files.files += \
    themes2

include(../common_bot.pri)
