TEMPLATE      = lib
CONFIG       += plugin gui meegotouch duicontrolpanel
# Qt 5: QtWidgets and QtX11Extras named here (CONFIG += qt again runs
# qt.prf before meegotouch.prf adds them)
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets x11extras

MOC_DIR	      = .moc
OBJECTS_DIR   = .objects

HEADERS       = privatemodeapplet.h \
                privatemodebrief.h

SOURCES       = privatemodeapplet.cpp \
                privatemodebrief.cpp

TARGET        = $$qtLibraryTarget(privatemodeapplet)
desktop.files += *.desktop
desktop.path = $$(DEBIAN_DESTDIR)/usr/lib/duicontrolpanel
target.path += $$(DEBIAN_DESTDIR)$$[QT_INSTALL_LIBS]/duicontrolpanel/applets

INSTALLS += target \
            desktop

