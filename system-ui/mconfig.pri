# Load more defines from the m_defines...
load(meegotouch_defines)
DEFINES += THEMEDIR=\\\"\"$$M_THEME_DIR\"\\\"

# For profiling with gprof:
#QMAKE_CXXFLAGS += -pg
#QMAKE_LFLAGS += -pg

QMAKE_CXXFLAGS += -g
QMAKE_LFLAGS += -g

# Qt 5 and Xlib in one file: qt5x11prelude.h before everything else
# (moc's predefined macros come from the compiler with these flags, but
# without the include path: moc goes without, as Qt 4's did)
greaterThan(QT_MAJOR_VERSION, 4) {
    QMAKE_CXXFLAGS += -include $$PWD/qt5x11prelude.h
    CONFIG += no_moc_predefs
    # Qt 4's QtGui linked Xlib, Qt 5's does not
    LIBS *= -lX11
}

# Directories
unix {
    SYSTEMUI_THEMES_DIR = /usr/share/themes/base/meegotouch/sysuid
}
mac {
     # Do mac stuff here
}
win32 {
     # Do win32 stuff here
}
