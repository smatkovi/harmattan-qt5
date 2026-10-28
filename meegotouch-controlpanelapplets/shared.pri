
# no rpath (Nokia's "-Wl" in its place made one flag of nothing for Qt 4;
# appended to Qt 5's "-Wl,-rpath," it leaves -rpath without its value,
# which then takes the next linker option: Qt for FreeBSD has rpath dirs)
QMAKE_LFLAGS_RPATH =
QMAKE_LFLAGS += -Wl,--as-needed

defineTest(addSubDirs) {
    for(subdirs, 1) {
        entries = $$files($$subdirs)
        for(entry, entries) {
            name = $$replace(entry, [/\\\\], _)
            SUBDIRS += $$name
            eval ($${name}.subdir = $$entry)
            for(dep, 2):eval ($${name}.depends += $$replace(dep, [/\\\\], _))
            export ($${name}.subdir)
            export ($${name}.depends)
        }
    }
    export (SUBDIRS)
}


# Qt 5: QtWidgets and QtX11Extras named here (the meegotouch features
# would add them after qt.prf has run), Q_WS_X11, Xlib linked (Qt 4's
# QtGui brought it), no -Werror
greaterThan(QT_MAJOR_VERSION, 4) {
    QT *= widgets x11extras
    DEFINES *= Q_WS_X11
    LIBS *= -lX11
    QMAKE_CXXFLAGS -= -Werror
}
