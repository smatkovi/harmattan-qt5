
include(../prefix.pri)

TEMPLATE = lib
CONFIG += plugin
TARGET = meegofeedback-dummy
QT = core
QT -= gui
QMAKE_LFLAGS += "-Wl,--as-needed"

INCLUDEPATH += ../src/lib

HEADERS = mfbackenddummy.h
SOURCES = mfbackenddummy.cpp

target.path = $$MF_PREFIX/lib/meegofeedbackd
INSTALLS += target

check.target = check
check.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check

check-xml.target = check-xml
check-xml.CONFIG = recursive
QMAKE_EXTRA_TARGETS += check-xml

