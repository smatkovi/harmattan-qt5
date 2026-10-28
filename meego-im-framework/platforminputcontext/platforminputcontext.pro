# Qt 5.1.1's Maliit platform input context (qtbase's
# src/plugins/platforminputcontexts/maliit), built outside qtbase: Qt 5
# applications with QT_IM_MODULE=maliit talk to meego-im-uiserver
TEMPLATE = lib
TARGET = maliitplatforminputcontextplugin
CONFIG += plugin
QT += dbus gui-private

SOURCES += qmaliitplatforminputcontext.cpp \
           serverproxy.cpp \
           serveraddressproxy.cpp \
           contextadaptor.cpp \
           main.cpp

HEADERS += qmaliitplatforminputcontext.h \
           serverproxy.h \
           serveraddressproxy.h \
           contextadaptor.h

OTHER_FILES += maliit.json

target.path = $$[QT_INSTALL_PLUGINS]/platforminputcontexts
INSTALLS += target
