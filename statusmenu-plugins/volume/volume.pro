PLUGIN = volume
include(../plugin.pri)
PKGCONFIG += libpulse libpulse-mainloop-glib
HEADERS += volumeplugin.h pulsevolume.h
SOURCES = volumeplugin.cpp pulsevolume.cpp
