# Load more defines from the dui_defines.
load(meegotouch_defines)

# Add global libdui includes
INCLUDEPATH += $$M_INSTALL_HEADERS

# Check for testability features, should they be compiled in or not?
!isEqual(TESTABILITY,"off") {
    DEFINES += WINDOW_DEBUG
}

# Check for remote control interface
isEqual(REMOTE_CONTROL,"on") {
    DEFINES += REMOTE_CONTROL
}

# Qt 5: the graphics view and QApplication in QtWidgets, QGLWidget in
# QtOpenGL, QX11Info in QtX11Extras; Q_WS_X11 is no longer defined
QT *= widgets opengl x11extras
DEFINES *= Q_WS_X11
# no -Werror (today's compilers and Qt 5's deprecations warn)
QMAKE_CXXFLAGS -= -Werror
# Xlib's Bool macro breaks moc's code, see x11bool.h
INCLUDEPATH += $$PWD
# Qt 4's QtGui linked Xlib and its extensions, Qt 5's does not
LIBS *= -lX11 -lXext -lXfixes

# Compositor components only
VERSION = 1.1.3
