QMAKE_LFLAGS_RPATH += -Wl
QMAKE_LFLAGS += -Wl,--as-needed

MOC_DIR = ../.moc

include(common.pri)
include(check.pri)
