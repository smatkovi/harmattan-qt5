
include(../../prefix.pri)

TEMPLATE = lib
DESTDIR = ../..
TARGET = meegofeedback

CONFIG += shared thread
DEFINES += THEME_DIR_PREFIX=\\\"$$MF_PREFIX/share/themes\\\"
DEFINES += MF_LIB_DIR=\\\"$$MF_PREFIX/lib/meegofeedbackd\\\"
QT -= gui
QT += concurrent

QMAKE_CXXFLAGS_RELEASE += -finline-functions
QMAKE_LFLAGS += "-Wl,--as-needed"

CONFIG(release, debug|release){
    DEFINES += QT_DEBUG_OUTPUT
    DEFINES += QT_WARNING_OUTPUT
} else {
    DEFINES += MF_DEBUG
}

contains( COV_OPTION, on ) {
    include(../coverage.pri)
}

VERSION = 0.9.0

PUBLIC_HEADERS = \
                 mfbackendbase.h \
                 mfbackendinterface.h \
                 mffeedback.h \
                 mflog.h \
                 mfsession.h \
                 mfsourcebase.h \
                 mfsourceinterface.h \
                 mfutil.h \
                 mfcommon.h

HEADERS = \
          $$PUBLIC_HEADERS \
          mfconfig.h \
          mfmanager.h \
          mffeedbackhandle.h

SOURCES = \
          mfbackendbase.cpp \
          mfsourcebase.cpp \
          mfconfig.cpp \
          mffeedback.cpp \
          mffeedbackhandle.cpp \
          mflog.cpp \
          mfmanager.cpp \
          mfsession.cpp \
          mfutil.cpp

check.target = check
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check

check-xml.target = check-xml
check-xml.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check-xml

headers.path = $$MF_PREFIX/include/meegofeedback
headers.files = $$PUBLIC_HEADERS

target.path = $$MF_PREFIX/lib

install_prf.path = $$[QT_INSTALL_DATA]/mkspecs/features
install_prf.files = meegofeedback.prf

INSTALLS += target headers install_prf
