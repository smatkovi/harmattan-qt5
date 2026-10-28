TEMPLATE = subdirs
SUBDIRS = cxx
# the QML plugins are QtQuick 1 (QtDeclarative), not built with Qt 5
lessThan(QT_MAJOR_VERSION, 5): SUBDIRS += qml
