include(../prefix.pri)

# The audio backend for PulseAudio (written anew; see mfbackendpulse.h)
TEMPLATE = lib
CONFIG += plugin link_pkgconfig
TARGET = meegofeedback-pulse
QT = core
PKGCONFIG += libpulse
QMAKE_LFLAGS += "-Wl,--as-needed"

INCLUDEPATH += ../src/lib
LIBS += -L../ -lmeegofeedback

HEADERS = mfbackendpulse.h
SOURCES = mfbackendpulse.cpp

target.path = $$MF_PREFIX/lib/meegofeedbackd/non-threaded-backends
INSTALLS += target
