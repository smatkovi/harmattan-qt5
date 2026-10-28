TEMPLATE = subdirs
SUBDIRS = plainqt
# the others use Qt 4's QInputContext
lessThan(QT_MAJOR_VERSION, 5): SUBDIRS += twofields widgetproperties allinputmethods

# the GTK+ examples need the GTK+ input context, not built with Qt 5
lessThan(QT_MAJOR_VERSION, 5):system(pkg-config gtk+-2.0) {
    SUBDIRS += gtk2
}

lessThan(QT_MAJOR_VERSION, 5):system(pkg-config gtk+-3.0) {
    SUBDIRS += gtk3
}
