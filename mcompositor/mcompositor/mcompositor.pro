include(../meegotouch_config.pri)

TEMPLATE = app
INCLUDEPATH += ../src

LIBS += ../src/libmcompositor.so ../decorators/libdecorator/libdecorator.so

target.path += $$M_INSTALL_BIN
INSTALLS += target 

# Input
SOURCES += main.cpp

contains(DEFINES, WINDOW_DEBUG_ALOT) {
    HEADERS += xserverpinger.h
    SOURCES += xserverpinger.cpp
}

# Qt 5: QApplication and QGraphicsView are in QtWidgets, QX11Info in
# QtX11Extras
QT = core gui opengl widgets x11extras
