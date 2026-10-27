include(./config.pri)

CONFIG += ordered
TARGET = mim-settings-applet
TEMPLATE = subdirs
SUBDIRS = settings-applet tests translations

QMAKE_EXTRA_TARGETS += check-xml
check-xml.target = check-xml
check-xml.CONFIG = recursive

QMAKE_EXTRA_TARGETS += check
check.target = check
check.CONFIG = recursive

