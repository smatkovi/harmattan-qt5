TEMPLATE = subdirs
SUBDIRS  = system keyd
# the tests (they do not all link with Qt 5 yet): not with CONFIG+=NOTESTS
!NOTESTS: SUBDIRS += tests

include( doc/doc.pri )

PKGCONFIGFILES.files = qmsystem2.pc
PKGCONFIGFILES.path = /usr/lib/pkgconfig

QTCONFIGFILES.files = qmsystem2.prf
lessThan(QT_MAJOR_VERSION, 5): QTCONFIGFILES.path = /usr/share/qt4/mkspecs/features
else: QTCONFIGFILES.path = $$[QT_INSTALL_DATA]/mkspecs/features

INSTALLS += PKGCONFIGFILES QTCONFIGFILES
