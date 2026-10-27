include(../../meegotouch_config.pri)

# the N9's swipe written anew (see mswipegesture.h), an mcompositor
# extension in its plugin directory
TEMPLATE = lib
TARGET = mcompositor-swipe-gesture
CONFIG += plugin
QT += dbus

INCLUDEPATH += ../../src ../../decorators/libdecorator
LIBS += ../../src/libmcompositor.so

# Qt's headers before Xlib's in every file, moc without the compiler's
# predefined macros (they would bring Xlib's)
QMAKE_CXXFLAGS += -include $$PWD/prelude.h
CONFIG += no_moc_predefs

HEADERS += mswipegesture.h
SOURCES += mswipegesture.cpp

target.path = /usr/lib/mcompositor
INSTALLS += target
