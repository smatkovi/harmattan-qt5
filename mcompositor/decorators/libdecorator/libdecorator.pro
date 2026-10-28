include(../../meegotouch_config.pri)
TEMPLATE = lib
CONFIG += dll release
QT += opengl network dbus
TARGET = decorator

DBUS_ADAPTORS = mdecorator_dbus.xml
DBUS_INTERFACES = mdecorator_dbus.xml
QMAKE_QDBUSXML2CPP = $$[QT_HOST_BINS]/qdbusxml2cpp -i mabstractappinterface.h \$(if $(filter mdecorator_dbus_interface.%,\$@),-c MDecoratorInterface)
# Qt 5's qmake prepares qdbusxml2cpp itself and takes its flags from these
QDBUSXML2CPP_ADAPTOR_HEADER_FLAGS = -i mabstractappinterface.h
QDBUSXML2CPP_INTERFACE_HEADER_FLAGS = -i mabstractappinterface.h -c MDecoratorInterface
QDBUSXML2CPP_INTERFACE_SOURCE_FLAGS = -c MDecoratorInterface

publicHeaders.files = mabstractdecorator.h mrmi.h mabstractappinterface.h
dbusiface.files = mdecorator_dbus_interface.h
HEADERS += $${publicHeaders.files} mrmi.cpp
SOURCES += mabstractdecorator.cpp mrmi.cpp mabstractappinterface.cpp
PRE_TARGETDEPS += mdecorator_dbus_interface.h

publicHeaders.path = $$M_INSTALL_HEADERS/libdecorator
dbusiface.path = $${publicHeaders.path}
dbusiface.CONFIG += no_check_exist
INSTALLS += publicHeaders dbusiface
target.path = /usr/lib
INSTALLS += target
QMAKE_EXTRA_TARGETS += check
check.depends = $$TARGET
check.commands = $$system(true)
