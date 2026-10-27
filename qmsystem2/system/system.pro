TEMPLATE = lib
TARGET = qmsystem2
DEPENDPATH += .
INCLUDEPATH += .
DEFINES += SYSTEM_LIBRARY
QT = core network dbus

QMAKE_CXXFLAGS += -Wall -Wno-psabi
lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -Werror

CONFIG += link_pkgconfig
lessThan(QT_MAJOR_VERSION, 5) {
    PKGCONFIG += dsme dsme_dbus_if gconf-2.0 libiphb sensord timed
    CONFIG += qmsystem_sensord qmsystem_timed qmsystem_iphb
} else {
    # Away from the N9 (Maemo Leste, FreeBSD): DSME's names, GConf and
    # MCE's (on Leste) are there; the N9's sensor daemon, time daemon and
    # heartbeat are built in where they are found, the battery comes from
    # UPower or the kernel (qmbattery_upower.cpp) instead of BME
    PKGCONFIG += dsme dsme_dbus_if gconf-2.0
    packagesExist(sensord) {
        PKGCONFIG += sensord
        CONFIG += qmsystem_sensord
    }
    packagesExist(timed) {
        PKGCONFIG += timed
        CONFIG += qmsystem_timed
    }
    packagesExist(libiphb) {
        PKGCONFIG += libiphb
        CONFIG += qmsystem_iphb
    }
}

message("Compiling with mce support")
DEFINES += HAVE_MCE

linux-g++-maemo {
    message("Compiling with usb-moded-dev support")
    DEFINES += HAVE_USB_MODED_DEV
    PKGCONFIG += usb_moded

    message("Compiling with sysinfo support")
    DEFINES += HAVE_SYSINFO
    PKGCONFIG += sysinfo
} else {
    message("Compiling without usb-moded-dev support")
    message("Compiling without sysinfo support")
}

# DEFINES += HAVE_QMLOG
message("Compiling without qmlog support")

# Input
HEADERS += mainpage.h \
    msystemdbus_p.h \
    qmactivity.h \
    qmactivity_p.h \
    qmbattery.h \
    qmcabc.h \
    qmcallstate.h \
    qmcallstate_p.h \
    qmdevicemode.h \
    qmdevicemode_p.h \
    qmdisplaystate.h \
    qmdisplaystate_p.h \
    qmipcinterface_p.h \
    qmkeys.h \
    qmkeys_p.h \
    qmled.h \
    qmlocks.h \
    qmlocks_p.h \
    qmsysteminformation.h \
    qmsysteminformation_p.h \
    qmsystemstate.h \
    qmsystemstate_p.h \
    qmthermal.h \
    qmthermal_p.h \
    qmusbmode.h \
    qmusbmode_p.h \
    qmwatchdog.h \
    qmwatchdog_p.h \
    system_global.h 
SOURCES += qmactivity.cpp \
    qmcabc.cpp \
    qmcallstate.cpp \
    qmdevicemode.cpp \
    qmdisplaystate.cpp \
    qmipcinterface.cpp \
    qmkeys.cpp \
    qmled.cpp \
    qmlocks.cpp \
    qmsysteminformation.cpp \
    qmsystemstate.cpp \
    qmthermal.cpp \
    qmwatchdog.cpp \
    qmusbmode.cpp

qmsystem_sensord {
    HEADERS += qmaccelerometer.h \
        qmaccelerometer_p.h \
        qmals.h \
        qmals_p.h \
        qmcompass.h \
        qmcompass_p.h \
        qmmagnetometer.h \
        qmmagnetometer_p.h \
        qmorientation.h \
        qmorientation_p.h \
        qmproximity.h \
        qmproximity_p.h \
        qmrotation.h \
        qmrotation_p.h \
        qmsensor.h \
        qmsensor_p.h \
        qmtap.h \
        qmtap_p.h
    SOURCES += qmaccelerometer.cpp \
        qmals.cpp \
        qmcompass.cpp \
        qmmagnetometer.cpp \
        qmorientation.cpp \
        qmproximity.cpp \
        qmrotation.cpp \
        qmsensor.cpp \
        qmtap.cpp
}
qmsystem_timed {
    HEADERS += qmtime.h qmtime_p.h
    SOURCES += qmtime.cpp
} else {
    # without timed: time jumps from the kernel, the zone from /etc/localtime
    HEADERS += qmtime.h
    SOURCES += qmtime_notimed.cpp
}
qmsystem_iphb {
    HEADERS += qmheartbeat.h qmheartbeat_p.h
    SOURCES += qmheartbeat.cpp
} else {
    # without iphbd: a timer
    HEADERS += qmheartbeat.h
    SOURCES += qmheartbeat_noiphb.cpp
}

linux-g++-maemo {
    message("Compiling with bmeipc support")
    PKGCONFIG += bmeipc
    HEADERS += qmbattery_p.h
    SOURCES += qmbattery.cpp
} else:greaterThan(QT_MAJOR_VERSION, 4) {
    message("Compiling with the battery from UPower or the kernel")
    SOURCES += qmbattery_upower.cpp
} else {
    message("Compiling without bmeipc support")
    SOURCES += qmbattery_stub.cpp 
}

contextsubscriber { 
    DEFINES += PROVIDE_CONTEXT_INFO
    LIBS += -lcontextsubscriber
}
targetheaders.path = /usr/include/qmsystem2
targetheaders.files = $$HEADERS
target.path = /usr/lib/
INSTALLS += target \
    targetheaders
