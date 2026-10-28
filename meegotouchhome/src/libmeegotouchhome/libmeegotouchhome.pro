MOC_DIR = .moc
M_MGEN_OUTDIR = .gen
OBJECTS_DIR = .obj

include(../../m_targets.pri)
include(../../mconfig.pri)

SOURCES += library.cpp

TEMPLATE = lib
TARGET = meegotouchhome
CONFIG += meegotouch
# Qt 5: CONFIG += qt again (base.pri) runs qt.prf before meegotouch.prf,
# too early for the modules the latter adds
QT *= widgets x11extras
# the Events page: a sink of system-ui's notification manager
QT *= dbus
LIBS += -lnotificationsystem
DEFINES *= Q_WS_X11
INCLUDEPATH += .
DEPENDPATH += .

# For setting the coverage flag ON
contains(COV_OPTION, on) {
    LIBS += -lgcov
    QMAKE_CXXFLAGS += -ftest-coverage \
        -fprofile-arcs
}

system(m-servicefwgen -a com.meego.core.HomeScreen)

include(../../localconfig.pri)
include(base.pri)
include(files.pri)

# Input
headers.path += /usr/include/$$TARGET
headers.files += $$INSTALL_HEADERS
target.path += /usr/lib
INSTALLS += target \
    headers

CONFIG += link_pkgconfig
PKGCONFIG += xcomposite \
    contextsubscriber-1.0 \
    contentaction-0.1
QT += network \
    svg \
    dbus \
    xml
exists($$[QT_INSTALL_LIBS]/libQtOpenGL.so)|exists($$[QT_INSTALL_LIBS]/libQt5OpenGL.so):QT += opengl
DEFINES += APPLICATIONS_DIRECTORY=\'$$quote(\"/usr/share/applications/\")\'
DEFINES += M_XDG_DIR=\\\"\"$$M_XDG_DIR\"\\\"

contains(BENCHMARKS, on) {
    DEFINES += BENCHMARKS_ON
}
QMAKE_CXXFLAGS += \
    -g
QMAKE_CLEAN += *.gcov \
    ./.obj/*.gcno
QMAKE_EXTRA_TARGETS += check
check.commands = $$system(true)
QMAKE_EXTRA_TARGETS += check-xml
check-xml.commands = $$system(true)
